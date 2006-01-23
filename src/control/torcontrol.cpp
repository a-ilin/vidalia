/****************************************************************
 *  $Id$
 * 
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/
 
#include <QHostAddress>

#include "../config/vidaliasettings.h"
#include "torcontrol.h"

/** Default constructor */
TorControl::TorControl()
{
  /* For reasons currently unknown to me, QProcess saves some state
   * information between executions of a process. Consequently, if we started
   * Tor, it crashed, and then we tried to restart it, Vidalia would crash in
   * the QProcess code. So, we create a new TorProcess object each time we
   * start Tor and then destroy it when it stops. */
  _torProcess = 0;

  /* Plumb the appropriate socket signals */
  QObject::connect(&_controlConn, SIGNAL(connected()),
                   this, SLOT(onConnected()));
  QObject::connect(&_controlConn, SIGNAL(disconnected()),
                   this, SLOT(onDisconnected()));
  QObject::connect(&_controlConn, SIGNAL(torEvent(ControlReply)),
                   this, SLOT(onTorEvent(ControlReply)));
}

/** Default destructor */
TorControl::~TorControl()
{
  if (isConnected()) {
    disconnect();
  }
  if (isRunning()) {
    stop();
  }
}

/** Start the Tor process. Returns true if the process was successfully
 * started, otherwise returns false. */
bool
TorControl::start(QString *errmsg)
{
  VidaliaSettings settings;
  _torProcess = new TorProcess;
  
  /* Plumb the process signals */
  QObject::connect(_torProcess, SIGNAL(started()),
                   this, SLOT(onStarted()));
  QObject::connect(_torProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                   this, SLOT(onStopped(int, QProcess::ExitStatus)));
  
  /* Attempt to start the Tor process */
  if (!_torProcess->start(settings.getTorExecutable(),
                          settings.getTorArguments(), errmsg)) {
    /* Disconnect the signals for this TorProcess, cleanup and return  */
    QObject::disconnect(_torProcess, 0, 0, 0);
    delete _torProcess;
    _torProcess = 0;
    return false;
  }
  return true;
}

/** Emits a signal that the Tor process started */
void
TorControl::onStarted()
{
  emit started();
}

/** Stop the Tor process. */
bool
TorControl::stop(QString *errmsg)
{
  if (isConnected()) {
    disconnect();
  }
  return _torProcess->stop(errmsg);
}

/** Emits a signal that the Tor process stopped */
void
TorControl::onStopped(int exitCode, QProcess::ExitStatus exitStatus)
{
  delete _torProcess;
  _torProcess = 0;
  emit stopped(exitCode, exitStatus);
}

/** Detect if the Tor process is running. */
bool
TorControl::isRunning()
{
  if (_torProcess) {
    return (_torProcess->pid() != 0);
  }
  return false;
}

/** Connect to Tor's control port. The control port to use is determined by
 * Vidalia's configuration file. */
bool
TorControl::connect(QString *errmsg)
{
  VidaliaSettings settings;
  return _controlConn.connect(settings.getControlAddress(),
                              settings.getControlPort(), errmsg);
}

/** Emits a signal that the control socket successfully established a
 * connection to Tor. */
void
TorControl::onConnected()
{
  emit connected();
}

/** Disconnect from Tor's control port */
void
TorControl::disconnect()
{
  _controlConn.disconnect();
}

/** Emits a signal that the control socket disconnected from Tor */
void
TorControl::onDisconnected()
{
  emit disconnected();
}

/** Check if theh control socket is connected */
bool
TorControl::isConnected()
{
  return _controlConn.isValid();
}

/** Send a message to Tor and read the response */
bool
TorControl::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  return _controlConn.send(cmd, reply, errmsg);
}

/** Sends an authentication token to Tor. This must be done before sending 
 * any control commands to Tor. The syntax is:
 * 
 *   "AUTHENTICATE" [ SP 1*HEXDIG / QuotedString ] CRLF
 */
bool
TorControl::authenticate(QString *errmsg)
{
  VidaliaSettings settings;
  ControlCommand cmd("AUTHENTICATE", QString(settings.getAuthToken()));
  ControlReply reply;

  if (!send(cmd, reply, errmsg)) {
    return false;
  } else {
    ReplyLine line = reply.getLine();
    if (line.getStatus() != "250") {
      if (errmsg) {
        *errmsg = line.getMessage();
      }
      return false;
    }
  }
  return true;
}

/** Sends a GETINFO message to Tor based on the given map of keyvals. The
 * syntax is:
 * 
 *    "GETINFO" 1*(SP keyword) CRLF 
 */
bool
TorControl::getInfo(QHash<QString,QString> &map, QString *errmsg)
{
  ControlCommand cmd("GETINFO");
  ControlReply reply;

  /* Add the keys as arguments to the GETINFO message */
  foreach (QString key, map.keys()) {
    cmd.addArgument(key);
  }
 
  /* Ask Tor for the specified info values */
  if (!send(cmd, reply, errmsg)) {
    return false;
  } else {
    /* Parse the response for the returned values */
    foreach (ReplyLine line, reply.getLines()) {
      if (line.getStatus() != "250") {
        if (errmsg) {
          *errmsg = line.getMessage();
        }
        return false;
      }

      /* Split the "key=val" line and map them */
      QStringList keyval = line.getMessage().split("=");
      if (keyval.size() == 2) {
        map.insert(keyval.at(0), keyval.at(1));
      } else if (line.getMessage() != "OK") {
        if (errmsg) {
          *errmsg = QString("Invalid GETINFO response line. [%1]")
                                           .arg(line.getMessage());
        }
        return false;
      }
    }
  }
  return true;
}

/** Overloaded method to send a GETINFO command for a single info value */
bool
TorControl::getInfo(QString key, QString &val, QString *errmsg)
{
  QHash<QString,QString> map;
  map.insert(key.toLower(), "");

  if (getInfo(map, errmsg)) {
    val = map.value(key.toLower());
    return true;
  }
  return false;
}

/** Sends a signal to Tor */
bool
TorControl::signal(Signal sig, QString *errmsg)
{
  ControlCommand cmd("SIGNAL");
  ControlReply reply;
  QString sigtype;
  
  /* Convert the signal to the correct string */
  switch (sig) {
    case SignalReload:   sigtype = "RELOAD"; break;
    case SignalShutdown: sigtype = "SHUTDOWN"; break;
    case SignalDump:     sigtype = "DUMP"; break;
    case SignalDebug:    sigtype = "DEBUG"; break;
    case SignalHalt:     sigtype = "HALT"; break;
    default: return false;
  }
  
  /* Send and check the response */
  if (!send(cmd, reply, errmsg)) {
    return false;
  } else {
    ReplyLine line = reply.getLine();
    if (line.getStatus() != "250") {
      if (errmsg) {
        *errmsg = line.getMessage();
      }
      return false;
    }
  }
  return true;
}

/** Ask Tor for its version */
QString
TorControl::getTorVersion(QString *errmsg)
{
  QString ver;
  if (getInfo("version", ver, errmsg)) {
    return ver;
  }
  return "<unknown>";
}

/** Adds an event and target object to the event list. If the control socket
 * is connected, then this method will try to register the new event with Tor,
 * otherwise it simply adds the event and handler to the event list and
 * returns true. */ 
bool
TorControl::addEvent(TorEvents::TorEvent e, QString *errmsg)
{
  _torEvents.add(e);
  if (isConnected()) {
    return setEvents(errmsg);
  }
  return true;
}

/** Removes an event and target object from the event list. If the control
 * socket is connected, then this method will try to register the new list of
 * events with Tor. Otherwise, it simply adds the event and handler to the
 * event list and returns true. */
bool
TorControl::removeEvent(TorEvents::TorEvent e, QString *errmsg)
{
  _torEvents.remove(e);
  if (isConnected()) {
    return setEvents(errmsg);
  }
  return true;
}

/** Register for the events currently in the event list */
bool
TorControl::setEvents(QString *errmsg)
{
  ControlCommand cmd("SETEVENTS"); 
  ControlReply reply;

  /* Add each event to the argument list */
  foreach (TorEvents::TorEvent e, _torEvents.eventList()) {
    cmd.addArgument(TorEvents::toString(e));
  }

  /* Send the new list of events to Tor */
  if (!send(cmd, reply, errmsg)) {
    return false;
  } else {
    if (reply.getStatus() != "250") {
      if (errmsg) {
        *errmsg = reply.getLine().getMessage();
      }
      return false;
    }
  }
  return true;
}

/** Called when we receive a control event from Tor */
void
TorControl::onTorEvent(ControlReply event)
{
  _torEvents.handleEvent(event);
}

/** Connects a Tor event signal to the given slot */
bool
TorControl::connect(TorEvents::Signal s, QObject *obj, const char *slot)
{
  return _torEvents.connect(s, obj, slot);   
}

/** Disconnects a Tor event signal from the given slot */
bool
TorControl::disconnect(TorEvents::Signal s, QObject *obj, const char *slot)
{
  return _torEvents.disconnect(s, obj, slot);
}

