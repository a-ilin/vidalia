/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file TorSocket.h
** \brief A QTcpSocket that makes requests over Tor
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
  /** Emitted when a connection error has occurred. */
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

