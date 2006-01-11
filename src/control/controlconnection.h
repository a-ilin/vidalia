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

#ifndef _CONTROLCONNECTION_H
#define _CONTROLCONNECTION_H

#include <QTcpSocket>

#include "controlcommand.h"
#include "controlreply.h"

class ControlConnection : public QTcpSocket
{
public:
  /** Default constructor and destructor */
  ControlConnection();
  ~ControlConnection();

  /** Connect to Tor on the specified host and port */
  bool connect(QHostAddress addr, quint16 port, QString *errmsg = 0);
  
  /** Disconnect from Tor */
  bool disconnect(QString *errmsg = 0);

  /** Send a command to Tor */
  bool sendCommand(ControlCommand cmd, QString *errmsg = 0);

  /** Read a response from Tor */
  bool readReply(ControlReply &reply, QString *errmsg = 0);

  /** Send a command to Tor and immediately read the response */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);
};

#endif

