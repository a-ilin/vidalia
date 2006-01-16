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

  /* Construct the message pump and give it a control connection and TorEvents
   * object, used to translate and dispatch event messages sent by Tor */
  _messages = new MessagePump(&_controlConn, &_events);

  /* Plumb the signal relays between the TorEvents object and The Outside */
  QObject::connect(&_events, SIGNAL(bandwidth(quint64, quint64)),
                   this, SLOT(onBandwidthUpdate(quint64, quit64)),
                   Qt::DirectConnection);
  QObject::connect(&_events, SIGNAL(log(TorEvents::LogSeverity, QString)),
                   this, SLOT(onLogMessage(TorEvents::LogSeverity, QString)),
                   Qt::DirectConnection);
  QObject::connect(&_events, SIGNAL(circuit(quint64, TorEvents::CircuitStatus,
                                            QString)),
                   this, SLOT(onCircuitStatus(quint64, TorEvents::CircuitStatus,
                                              QString)),
                   Qt::DirectConnection);
  QObject::connect(&_events, SIGNAL(stream(quint64, TorEvents::StreamStatus,
                                           quint64, QString)),
                   this, SLOT(onStreamStatus(quint64, TorEvents::StreamStatus,
                                             quint64, QString)),
                   Qt::DirectConnection);
  
  /* Plumb the appropriate socket signals */
  QObject::connect(&_controlConn, SIGNAL(connected()),
                   this, SLOT(onConnected()));
  QObject::connect(&_controlConn, SIGNAL(disconnected()),
                   this, SLOT(onDisconnected()));
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
  if (_messages) {
    delete _messages;
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
  if ( _controlConn.connect(settings.getControlAddress(),
                            settings.getControlPort(), errmsg)
     ) {
    _messages->start();
    return true;
  }
  return false;
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
  _messages->stop();
  _controlConn.sendCommand(ControlCommand("QUIT"), 0);
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

/** Sends a message to Tor and discards the response */
bool
TorControl::send(ControlCommand cmd, QString *errmsg)
{
  ControlReply reply;
  return send(cmd, reply, errmsg);
}

/** Send a message to Tor and read the response */
bool
TorControl::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  return _messages->send(cmd, reply, errmsg);
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

  if (send(cmd, reply, errmsg)) {
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
  if (send(cmd, reply, errmsg)) {
  
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
      }
    }
  } else {
    /* Sending the control command failed */
    return false;
  }
  return true;
}

/** Overloaded method to send a GETINFO command for a single info value */
bool
TorControl::getInfo(QString key, QString &val, QString *errmsg)
{
  QHash<QString,QString> map;
  map.insert(key, "");

  if (getInfo(map, errmsg)) {
    val = map.value(key);
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

/** Adds an event to the event list and registers it with Tor. If registration
 * fails, then the event is NOT added to the event list. */
bool
TorControl::addEvent(TorEvents::Event e, QString *errmsg)
{
  QString event = TorEvents::toString(e);
  _eventList << event;
  if (!registerEvents(errmsg)) {
    /* Registering the event failed, so remove it from the list */
    _eventList.removeAt(_eventList.indexOf(event));
    return false;
  }
  return true;
}

/** Removes an event from the event list and unregisters it from Tor. If
 * unregistration fails, then the event is NOT removed from the event list. */
bool
TorControl::removeEvent(TorEvents::Event e, QString *errmsg)
{
  QString event = TorEvents::toString(e);
  if (_eventList.contains(event)) {
    _eventList.removeAt(_eventList.indexOf(event));
    if (!registerEvents(errmsg)) {
      /* Unregistration failed, so leave it in the list */
      _eventList << event;
      return false;
    }
  }
  return true;
}

/** Register for the events currently in the event list */
bool
TorControl::registerEvents(QString *errmsg)
{
  ControlCommand cmd("SETEVENTS", _eventList.join(" ")); 
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

/**
 * The methods below relay the appropriate signals from the TorEvents object.
 */
void
TorControl::onBandwidthUpdate(quint64 bytesIn, quint64 bytesOut)
{
  emit bandwidth(bytesIn, bytesOut);
}
void
TorControl::onLogMessage(TorEvents::LogSeverity severity, QString msg)
{
  emit log(severity, msg);
}
void
TorControl::onCircuitStatus(quint64 circId, 
                            TorEvents::CircuitStatus status, QString path)
{
  emit circuit(circId, status, path);
}
void
TorControl::onStreamStatus(quint64 streamId, TorEvents::StreamStatus status,
                           quint64 circId, QString target)
{
  emit stream(streamId, status, circId, target);
}

