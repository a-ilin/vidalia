/****************************************************************
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

/** 
 * \file torcontrol.cpp
 * \version $Id$
 */

#include <QHostAddress>

#include <config/torsettings.h>
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

  /** Create an instance of a connection to Tor's control interface and give
   * it an object to use to handle asynchronous events. */
  _controlConn = new ControlConnection(&_torEvents);

  /* Plumb the appropriate socket signals */
  QObject::connect(_controlConn, SIGNAL(connected()),
                   this, SLOT(onConnected()));
  QObject::connect(_controlConn, SIGNAL(connectFailed(QString)),
                   this, SLOT(onConnectFailed(QString)));
  QObject::connect(_controlConn, SIGNAL(disconnected()),
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
  delete _controlConn;
}

/** Start the Tor process. Returns true if the process was successfully
 * started, otherwise returns false. */
bool
TorControl::start(QString *errmsg)
{
  TorSettings settings;
  _torProcess = new TorProcess;
  
  /* Plumb the process signals */
  QObject::connect(_torProcess, SIGNAL(started()),
                   this, SLOT(onStarted()));
  QObject::connect(_torProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                   this, SLOT(onStopped(int, QProcess::ExitStatus)));
  QObject::connect(_torProcess, SIGNAL(log(QString, QString)),
                   this, SLOT(onLogStdout(QString, QString)));

  /* Attempt to start the Tor process */
  if (!_torProcess->start(settings.getExecutable(),
                          settings.getArguments(), errmsg)) {
    /* Disconnect the signals for this TorProcess, cleanup and return  */
    closeTorProcess();
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
  if (!isRunning()) {
    return true;
  }
  return _torProcess->stop(errmsg);
}

/** Emits a signal that the Tor process stopped */
void
TorControl::onStopped(int exitCode, QProcess::ExitStatus exitStatus)
{
  closeTorProcess();
  disconnect();
  emit stopped(exitCode, exitStatus);
}

/** Disconnect signals from _torProcess and clean up after it. */
void
TorControl::closeTorProcess()
{
  if (_torProcess) {
    QObject::disconnect(_torProcess, 0, 0, 0);
    delete _torProcess;
    _torProcess = 0;
  }
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
void
TorControl::connect()
{
  TorSettings settings;
  _controlConn->connect(settings.getControlAddress(),
                        settings.getControlPort());
}

/** Emits a signal that the control socket successfully established a
 * connection to Tor. */
void
TorControl::onConnected()
{
  QString errmsg;
 
  /* Authenticate and register for any pertinent asynchronous events. */
  if (!authenticate(&errmsg) || !setEvents(&errmsg)) {
    emit connectFailed(errmsg);
    stop();
    return;
  }
  
  /* Let interested parties know that the control socket connected */
  emit connected();
  emit connected(true);
  /* The control socket is connected, so we can stop reading from stdout */
  if (_torProcess) {
    _torProcess->closeStdout();
  }
}

/** Emits a signal that the control connection to Tor failed. */
void
TorControl::onConnectFailed(QString errmsg)
{
  emit connectFailed(errmsg);
}

/** Disconnect from Tor's control port */
void
TorControl::disconnect()
{
  if (_controlConn->isConnected()) {
    _controlConn->disconnect();
  }
}

/** Emits a signal that the control socket disconnected from Tor */
void
TorControl::onDisconnected()
{
  if (_torProcess) {
    _torProcess->openStdout();
  }
  emit disconnected();
  emit connected(false);
}

/** Check if theh control socket is connected */
bool
TorControl::isConnected()
{
  return _controlConn->isConnected();
}

/** Send a message to Tor and reads the response. If Vidalia was unable to
 * send the command to Tor or read its response, false is returned. If the
 * response was read and the status code is not 250 OK, false is also
 * returned. */
bool
TorControl::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  if (_controlConn->send(cmd, reply, errmsg)) {
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
  TorSettings settings;
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
TorControl::signal(TorSignal::Signal sig, QString *errmsg)
{
  ControlCommand cmd("SIGNAL");
  cmd.addArgument(TorSignal::toString(sig));
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
  return "<Unavailable>";
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
      arg += "=\"" + value + "\"";
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
TorControl::resetConf(QStringList keys, QString *errmsg)
{
  ControlCommand cmd("RESETCONF");

  /* Add each key to the argument list */
  foreach (QString key, keys) {
    cmd.addArgument(key);
  }
  return send(cmd, errmsg);
}

/** Tells Tor to reset a single given configuration key back to its default
 * value. */
bool
TorControl::resetConf(QString key, QString *errmsg)
{
  return resetConf(QStringList() << key, errmsg);
}

/** Gets the descriptor for the specified ID. If the ID has a "!" before it,
 * indicating that the associated router is unresponsive, it will be stripped
 * first. */
RouterDescriptor
TorControl::getRouterDescriptor(QString id, QString *errmsg)
{
  ControlCommand cmd("GETINFO", "desc/id/" + id.mid(id.indexOf("!")+1));
  ControlReply reply;
 
  if (send(cmd, reply, errmsg)) {
    return RouterDescriptor(id, reply.getData());
  }
  return RouterDescriptor(id);
}

/** Gets a list of RouterDescriptor objects for all routers that Tor currently
 * knows about. */
QList<RouterDescriptor>
TorControl::getRouterList(QString *errmsg)
{
  QList<RouterDescriptor> descList;
  QStringList idList = getRouterIDList(errmsg);

  /* Get the descriptor for each ID and add it to the list */
  foreach (QString id, idList) {
    descList << getRouterDescriptor(id, errmsg);
  }
  return descList;
}

/** Gets a list of router IDs for all routers Tor knows about. */
QStringList
TorControl::getRouterIDList(QString *errmsg)
{
  ControlCommand cmd("GETINFO", "network-status");
  ControlReply reply;
  QStringList idList;

  if (send(cmd, reply, errmsg)) {
    QString routerIDs = reply.getMessage().remove(0,qstrlen("network-status="));

    /* Split the list of router IDs up */
    QStringList routers = routerIDs.split(" ");
    foreach (QString router, routers) {
      /* A router ID may be of the form <name>=$<ID>, $<ID>, or <ID> */
      QString id = router.mid(router.indexOf("=")+1);
      id = id.replace("$", "");
      /* A "!" before <name> or <ID> means "unresponsive" */
      if (router.startsWith("!") && !id.startsWith("!")) {
        id.prepend("!");
      }
      /* Get the descriptor for this router ID and add it to the list. */
      idList << id;
    }
  }
  return idList;
}

