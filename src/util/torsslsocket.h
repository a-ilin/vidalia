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
 * \file torsslsocket.h
 * \version $Id: /local/vidalia/trunk/src/util/torsocket.h 1564 2006-12-26T06:06:04.965088Z edmanm  $
 * \brief A QSslSocket that makes encrypted requests over Tor
 */

#ifndef _TORSSLSOCKET_H
#define _TORSSLSOCKET_H

#include <QtGlobal>
#if QT_VERSION >= 0x040300
/* XXX: We should do some autoconf magic to define a HAVE_QSSLSOCKET_H
 * appropriately. */
#include <QSslSocket>
#include <QHostAddress>


class TorSslSocket : public QSslSocket
{
  Q_OBJECT
  
public:
  /** Constructor. */
  TorSslSocket(const QHostAddress &socksAddr,
               quint16 socksPort, QObject *parent = 0);

  /** Connects to the specified hostname and port via Tor. */
  void connectToRemoteHost(const QString &remoteHost, quint16 remotePort,
                           bool encrypted);

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
  /** Called when an encrypted connection has been established to the remote
   * host. */
  void onEncrypted();
  /** Handles the server's response part of a Socks4a handshake. */
  void onHandshakeResponse();
  /** Called when a connection error has occurred. */
  void onError(QAbstractSocket::SocketError error);
  /** Called when one or more SSL errors occur on the socket. */
  void onSslErrors(const QList<QSslError> &errors);
  
private:
  /** Sends the client part of a Socks4a handshake with a proxy server. */
  void sendSocksHandshake(const QString &remoteHost, quint16 remotePort);
  
  QHostAddress _socksAddr; /**< Address of Tor's SOCKS listener. */
  QString _remoteHost;     /**< Remote hostname. */
  quint16 _socksPort;      /**< Port of Tor's SOCKS listener. */
  quint16 _remotePort;     /**< Remote host port. */
  bool    _encrypted;      /**< Set to true if the connection to the remote
                                host should be encrypted. */
};

#endif
#endif

