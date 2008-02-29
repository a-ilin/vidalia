/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file torsslsocket.cpp
** \version $Id: /local/vidalia/trunk/src/util/torsocket.cpp 1564 2006-12-26T06:06:04.965088Z edmanm  $
** \brief A QSslSocket that makes encrypted requests over Tor
*/

#include "torsslsocket.h"

#include <QDataStream>
#include <QStringList>


#define SOCKS_VERSION             0x04 /**< SOCKS version. */
#define SOCKS_CONNECT             0x01 /**< SOCKS connect command ID. */
#define SOCKS_FAKE_IP             0x00000001 /**< Bogus IP. */
#define SOCKS_RESPONSE_LEN        0x08 /**< SOCKS server response length. */
#define SOCKS_RESPONSE_VERSION    0x00 /**< SOCKS server response version. */
#define SOCKS_CONNECT_STATUS_OK   0x5A /**< SOCKS server response status. */


/** Constructor. */
TorSslSocket::TorSslSocket(const QHostAddress &socksAddr,
                           quint16 socksPort, QObject *parent)
: QSslSocket(parent),
  _socksAddr(socksAddr),
  _socksPort(socksPort)
{
  QObject::connect(this, SIGNAL(sslErrors(QList<QSslError>)),
                   this, SLOT(onSslErrors(QList<QSslError>)));
  QObject::connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
                   this, SLOT(onError(QAbstractSocket::SocketError)));
  QObject::connect(this, SIGNAL(readyRead()),
                   this, SLOT(onHandshakeResponse()));
  QObject::connect(this, SIGNAL(connected()),
                   this, SLOT(connectedToProxy()));
  QObject::connect(this, SIGNAL(encrypted()),
                   this, SLOT(onEncrypted()));
}

/** Connects to the specified hostname and port via Tor. */
void
TorSslSocket::connectToRemoteHost(const QString &remoteHost, quint16 remotePort,
                                  bool encrypted)
{
  _remoteHost = remoteHost;
  _remotePort = remotePort;
  _encrypted  = encrypted;
  QTcpSocket::connectToHost(_socksAddr, _socksPort);
}

/** Called when a connection error has occurred. */
void
TorSslSocket::onError(QAbstractSocket::SocketError error)
{
  Q_UNUSED(error);
  emit socketError(errorString());
}

/** Called when one or more SSL errors occur on the socket. */
void
TorSslSocket::onSslErrors(const QList<QSslError> &errors)
{
  QStringList errorStrings;
  foreach (QSslError error, errors) {
    errorStrings << "\"" + error.errorString() + "\"";
  }
  emit socketError(errorStrings.join(","));
}

/** Called when a connection has been established to the proxy host and starts
 * a Socks4a handshake. */
void
TorSslSocket::connectedToProxy()
{
  sendSocksHandshake(_remoteHost, _remotePort);
}

/** Called when an encrypted connection has been established to the remote
 * host. */
void
TorSslSocket::onEncrypted()
{
  emit connectedToRemoteHost();
}

/** Sends the first part of a Socks4a handshake, using the remote hostname and
 * port specified in the previous call to connectToHost(). The message should
 * be formatted as follows:
 *
 *   0x04                 (socks version)
 *   0x01                 (connect)
 *   PORT                 (two bytes, most significant byte first)
 *   0x00 0x00 0x00 0x01  (fake IP address: tells proxy to use SOCKS4a)
 *   0x00                 (empty username field)
 *   HOSTNAME             (target hostname)
 *   0x00                 (marks the end of the hostname field)
 */
void
TorSslSocket::sendSocksHandshake(const QString &remoteHost, quint16 remotePort)
{
  QDataStream sock(this);
  sock << (quint8)SOCKS_VERSION;
  sock << (quint8)SOCKS_CONNECT;
  sock << (quint16)remotePort;
  sock << (quint32)SOCKS_FAKE_IP;
  sock << (quint8)0;
  sock.writeRawData(qPrintable(remoteHost), remoteHost.length());
  sock << (quint8)0;
}

/** Handles the second half of the handshake, received from the SOCKS 
 * proxy server. The response should be formatted as follows: 
 * 
 *    0x00                 (response version)
 *    STATUS               (0x5A means success; other values mean failure)
 *    PORT                 (not set)
 *    ADDRESS              (not set)
 */
void
TorSslSocket::onHandshakeResponse()
{
  QByteArray response;
  if (bytesAvailable() >= SOCKS_RESPONSE_LEN) {
    /* We've received our response, so stop waiting for it. */
    QObject::disconnect(this, SIGNAL(readyRead()),
                        this, SLOT(onHandshakeResponse()));
    
    /* Read the 8-byte response off the socket. */
    response = read(SOCKS_RESPONSE_LEN);
    
    /* Check to make sure we got a good response from the proxy. */
    if ((uchar)response[0] == (uchar)SOCKS_RESPONSE_VERSION &&
        (uchar)response[1] == (uchar)SOCKS_CONNECT_STATUS_OK) {
      if (_encrypted) {
        /* Connection status was okay, so start client encryption. */
        /* We first need to set the peer name to the intended remote host,
         * otherwise Qt will use the proxy (e.g., 127.0.0.1) as the peer name
         * when validating the server certificate. */
        setPeerName(_remoteHost);
        startClientEncryption();
      } else {
        /* Caller wanted an unencrypted, unauthenticated, uncool conn. */
        emit connectedToRemoteHost();
      }
    } else {
      /* Remote connection failed, so close the connection to the proxy. */
      disconnectFromHost();
    }
  }
}

