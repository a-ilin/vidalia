/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to
**  the terms described in the LICENSE file.
*/

/*
** \file controlsocket.h
** \version $Id$
** \brief Socket used to connect to Tor's control interface
*/

#ifndef _CONTROLSOCKET_H
#define _CONTROLSOCKET_H

#include <QTcpSocket>

#include "controlcommand.h"
#include "controlreply.h"


class ControlSocket : public QTcpSocket
{
  Q_OBJECT

public:
  /** Default constructor. */
  ControlSocket();

  /** Send a command to Tor */
  bool sendCommand(ControlCommand cmd, QString *errmsg = 0);
  /** Read a response from Tor */
  bool readReply(ControlReply &reply, QString *errmsg = 0);

  /** Returns true if the control socket is connected and ready to send or
   * receive. */
  bool isConnected();
  
  /** Returns the string description of <b>error</b>. */
  static QString toString(const QAbstractSocket::SocketError error);
  
protected:
  /** Reads line data off the socket in chunks. */
  bool readLineData(QString &line, QString *errmsg = 0);
  /** Reads a line of data from the socket (blocking) */
  bool readLine(QString &line, QString *errmsg = 0);
};

#endif

