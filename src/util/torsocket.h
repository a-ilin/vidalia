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
 * \file torsocket.h
 * \version $Id$
 * \brief A QTcpSocket that makes requests over Tor
 */

#ifndef _TORSOCKET_H
#define _TORSOCKET_H

#include <QTcpSocket>
#include <QHostAddress>


class TorSocket : public QTcpSocket
{
  Q_OBJECT
  
public:
  /** Constructor. */
  TorSocket(const QHostAddress &socksAddr,
              quint16 socksPort, QObject *parent = 0);

  /** Connects to the specified hostname and port via Tor. */
  void connectToRemoteHost(const QString &remoteHost, quint16 remotePort);

signals:
  /** Emitted when a connection has been established through Tor to the remote
   * host specified in a prior call to connectToHost(). */
  void connectedToRemoteHost();
  /** Emmitted when a connection error has occurred. */
  void socketError(QString errmsg);
  
private slots:
  /** Called when the socket is connected to the proxy and sends our
   * half of a Socks4a handshake. */
  void connectedToProxy();
  /** Handles the server's response part of a Socks4a handshake. */
  void onHandshakeResponse();
  /** Called when a connection error has occurred. */
  void onError(QAbstractSocket::SocketError error);

private:
  /** Sends the client part of a Socks4a handshake with a proxy server. */
  void sendSocksHandshake(const QString &remoteHost, quint16 remotePort);
  
  QHostAddress _socksAddr; /**< Address of Tor's SOCKS listener. */
  QString _remoteHost;     /**< Remote hostname. */
  quint16 _socksPort;      /**< Port of Tor's SOCKS listener. */
  quint16 _remotePort;     /**< Remote host port. */
};

#endif

