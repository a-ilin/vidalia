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
}

/** Default destructor */
TorControl::~TorControl()
{
}

/** Start the Tor process. Returns true if the process was successfully
 * started, otherwise returns false. */
bool
TorControl::startTor(QString *errmsg)
{
  VidaliaSettings settings;
  return _torProcess.start(settings.getTorExecutable(),
                           settings.getTorArguments(), errmsg);
}


/** Stop the Tor process. */
void
TorControl::stopTor()
{
  _torProcess.stop();
}

/** Detect if the Tor process is running. */
bool
TorControl::torIsRunning()
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

/** Disconnect from Tor's control port */
void
TorControl::disconnect()
{
  _controlConn.disconnect();
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
    if (line.getStatus() == "250") {
      return true;
    }
  }
  return false;
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
        *errmsg = line.getMessage();
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

/** Ask Tor for its version */
QString
TorControl::getTorVersion(QString *errmsg)
{
  QString ver("<unknown>");
  getInfo("version", ver, errmsg);
  return ver;
}

