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
 * \file controlsocket.h
 * \version $Id: $
 */

#ifndef _CONTROLSOCKET_H
#define _CONTROLSOCKET_H

#include <QTcpSocket>
#include <QHostAddress>

#include "controlcommand.h"
#include "controlreply.h"

/** Give up after waiting three seconds for the control socket to connect to
 * Tor. */
#define CONN_TIMEOUT  3000

/** Timeout reads in 250ms. We can set this to a short value because if there
 * isn't any data to read, we want to return anyway. */
#define READ_TIMEOUT  250


class ControlSocket : public QTcpSocket
{
  Q_OBJECT

public:
  /** Default constructor and destructor */
  ControlSocket();

  /** Connect to Tor on the specified host and port */
  bool connect(QHostAddress addr, quint16 port, QString *errmsg = 0);
  /** Disconnect from Tor */
  bool disconnect(QString *errmsg = 0);
  
  /** Send a command to Tor */
  bool sendCommand(ControlCommand cmd, QString *errmsg = 0);
  /** Read a response from Tor */
  bool readReply(ControlReply &reply, QString *errmsg = 0);

  /** Returns true if the control socket is connected and ready to send or
   * receive. */
  bool isConnected();

protected:
  /** Reads line data off the socket in chunks. */
  bool readLineData(QString &line, QString *errmsg = 0);
  /** Reads a line of data from the socket (blocking) */
  bool readLine(QString &line, QString *errmsg = 0);
  
private:
  /** Specifies a version of Tor's Control Protocol */
  enum ProtocolVersion {
    VersionUnknown = -1,  /**< Unknown version. */
    Version0 = 0,         /**< Protocol V0 (deprecated) */
    Version1 = 1          /**< Protocol V1 */
  };
  /** Returns the version of Tor's control protocol being spoken on this
   * socket. */
  ProtocolVersion protocolVersion();
};

#endif

