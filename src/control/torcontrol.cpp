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
  /* Plumb the process signals */
  QObject::connect(&_torProcess, SIGNAL(started()),
                   this, SLOT(onStarted()));
  QObject::connect(&_torProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                   this, SLOT(onStopped(int, QProcess::ExitStatus)));
  
  /* Plumb the appropriate socket signals */
  QObject::connect(&_controlConn, SIGNAL(connected()),
                   this, SLOT(onConnected()));
  QObject::connect(&_controlConn, SIGNAL(disconnected()),
                   this, SLOT(onDisconnected()));
}

/** Default destructor */
TorControl::~TorControl()
{
}

/** Start the Tor process. Returns true if the process was successfully
 * started, otherwise returns false. */
bool
TorControl::start(QString *errmsg)
{
  VidaliaSettings settings;
  return _torProcess.start(settings.getTorExecutable(),
                           settings.getTorArguments(), errmsg);
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
  return _torProcess.stop(errmsg);
}

/** Emits a signal that the Tor process stopped */
void
TorControl::onStopped(int exitCode, QProcess::ExitStatus exitStatus)
{
  emit stopped(exitCode, exitStatus);
}

/** Detect if the Tor process is running. */
bool
TorControl::isRunning()
{
  return (_torProcess.pid() != 0);
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
  return (_controlConn.state() == QAbstractSocket::ConnectedState);
}

/** Sends an authentication token to Tor. This must be done before sending 
 * any control commands to Tor. The syntax is:
 * 
 *   "AUTHENTICATE" [ SP 1*HEXDIG / QuotedString ] CRLF
 */
bool
TorControl::authenticate(QByteArray token, QString *errmsg)
{
  ControlCommand cmd("AUTHENTICATE", QString(token));
  ControlReply reply;

  if (_controlConn.send(cmd, reply, errmsg)) {
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
  if (_controlConn.send(cmd, reply, errmsg)) {
  
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
    case Reload:   sigtype = "RELOAD"; break;
    case Shutdown: sigtype = "SHUTDOWN"; break;
    case Dump:     sigtype = "DUMP"; break;
    case Debug:    sigtype = "DEBUG"; break;
    case Halt:     sigtype = "HALT"; break;
    default: return false; break;
  }
  
  /* Send and check the response */
  if (_controlConn.send(cmd, reply, errmsg)) {
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

