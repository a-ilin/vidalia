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
  _messages = new MessagePump(&_controlConn, &_torEvents);

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
  QObject::connect(_torProcess, SIGNAL(log(QString, QString)),
                   this, SLOT(onLogStdout(QString, QString)));

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
  disconnect();
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

/** Called when Tor has printed a log message to stdout. */
void
TorControl::onLogStdout(QString severity, QString message)
{
  LogEvent::Severity s = LogEvent::toSeverity(severity);
  _torEvents.dispatch(TorEvents::toTorEvent(s), new LogEvent(s, message));
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
  /* Start the message pump */
  _messages->start();
  /* Let interested parties know that the control socket connected */
  emit connected();
  /* The control socket is connected, so we can stop reading from stdout */
  _torProcess->logStdout(false);
}

/** Disconnect from Tor's control port */
void
TorControl::disconnect()
{
  _messages->stop();
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

/** Send a message to Tor and reads the response. If Vidalia was unable to
 * send the command to Tor or read its response, false is returned. If the
 * response was read and the status code is not 250 OK, false is also
 * returned. */
bool
TorControl::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  if (_messages->send(cmd, reply, errmsg)) {
    if (reply.getStatus() == "250") {
      return true;
    }
    if (errmsg) {
      *errmsg = reply.getMessage();
    }
  }
  return false;
}

/** Sends a message to Tor and discards the response. */
bool
TorControl::send(ControlCommand cmd, QString *errmsg)
{
  ControlReply reply;
  return send(cmd, reply, errmsg);
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
  return send(cmd, errmsg);
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
      /* Split the "key=val" line and map them */
      QStringList keyval = line.getMessage().split("=");
      if (keyval.size() == 2) {
        map.insert(keyval.at(0), keyval.at(1));
      }
    }
    return true;
  }
  return false;
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
  return send(cmd, errmsg); 
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

/** Sets an event and its handler. If add is true, then the event is added,
 * otherwise it is removed. If set is true, then the given event will be
 * registered with Tor. */
bool
TorControl::setEvent(TorEvents::TorEvent e, QObject *obj, 
                     bool add, bool set, QString *errmsg)
{
  if (add) {
    _torEvents.add(e, obj);
  } else {
    _torEvents.remove(e, obj);
  }
  if (set && isConnected()) {
    return setEvents(errmsg);
  }
  return true;
}

/** Registers for a set of logging events according to the given filter. If
 * the control socket is currently connected, this method will try to register
 * the log events with Tor, otherwise it will simply return true. */
bool
TorControl::setLogEvents(uint filter, QObject *obj, QString *errmsg)
{
  setEvent(TorEvents::LogError , obj, filter & LogEvent::TorError , false);
  setEvent(TorEvents::LogWarn  , obj, filter & LogEvent::TorWarn  , false);
  setEvent(TorEvents::LogNotice, obj, filter & LogEvent::TorNotice, false);
  setEvent(TorEvents::LogInfo  , obj, filter & LogEvent::TorInfo  , false);
  setEvent(TorEvents::LogDebug , obj, filter & LogEvent::TorDebug , false);
  return (isConnected() ? setEvents(errmsg) : true);
}

/** Register for the events currently in the event list */
bool
TorControl::setEvents(QString *errmsg)
{
  ControlCommand cmd("SETEVENTS"); 

  /* Add each event to the argument list */
  foreach (TorEvents::TorEvent e, _torEvents.eventList()) {
    cmd.addArgument(TorEvents::toString(e));
  }
  return send(cmd, errmsg);
}

/** Sets each configuration key in \emph map to the value associated with its
 * key. */
bool
TorControl::setConf(QHash<QString,QString> map, QString *errmsg)
{
  ControlCommand cmd("SETCONF");
  QString arg, value;
  
  /* Add each keyvalue to the argument list */
  foreach (QString key, map.keys()) {
    arg = key;
    value = map.value(key);
    if (value.length() > 0) {
      arg += "=" + value;
    }
    cmd.addArgument(arg);
  }
  return send(cmd, errmsg); 
}

/** Sets a single configuration key to the given value. */
bool
TorControl::setConf(QString key, QString value, QString *errmsg)
{
  QHash<QString,QString> map;
  map.insert(key, value);
  return setConf(map, errmsg);
}

/** Gets a set of configuration keyvalues and stores them in \emph map. */
bool
TorControl::getConf(QHash<QString,QString> &map, QString *errmsg)
{
  ControlCommand cmd("GETCONF");
  ControlReply reply;

  /* Add the keys as arguments to the GETINFO message */
  foreach (QString key, map.keys()) {
    cmd.addArgument(key);
  }
 
  /* Ask Tor for the specified info values */
  if (send(cmd, reply, errmsg)) {
    /* Parse the response for the returned values */
    foreach (ReplyLine line, reply.getLines()) {
      /* Split the "key=val" line and map them */
      QStringList keyval = line.getMessage().split("=");
      if (keyval.size() == 2) {
        map.insert(keyval.at(0), keyval.at(1));
      }
    }
    return true;
  }
  return false;
}

/** Gets a single configuration keyvalue. */
bool
TorControl::getConf(QString key, QString &value, QString *errmsg)
{
  QHash<QString,QString> map;
  map.insert(key, "");

  if (getConf(map, errmsg)) {
    value = map.value(key);
    return true;
  }
  return false;
}

/** Asks Tor to save the current configuration to its torrc. */
bool
TorControl::saveConf(QString *errmsg)
{
  ControlCommand cmd("SAVECONF");
  return send(cmd, errmsg);
}

/** Tells Tor to reset the given configuration keys back to defaults. */
bool
TorControl::resetConf(QList<QString> keys, QString *errmsg)
{
  ControlCommand cmd("RESETCONF");

  /* Add each key to the argument list */
  foreach (QString key, keys) {
    cmd.addArgument(key);
  }
  return send(cmd, errmsg);
}

