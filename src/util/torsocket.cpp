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
 * \file torsocket.cpp
 * \version $Id$ 
 */

#include <QDataStream>

#include "torsocket.h"

#define SOCKS_VERSION             0x04 /**< SOCKS version. */
#define SOCKS_CONNECT             0x01 /**< SOCKS connect command ID. */
#define SOCKS_FAKE_IP             0x00000001 /**< Bogus IP. */
#define SOCKS_RESPONSE_LEN        0x08 /**< SOCKS server response length. */
#define SOCKS_RESPONSE_VERSION    0x00 /**< SOCKS server response version. */
#define SOCKS_CONNECT_STATUS_OK   0x5A /**< SOCKS server response status. */
#define LOCAL_CONNECT_TIMEOUT     3000 /**< Time to wait for a connection to
                                            the local SOCKS proxy. */

/** Constructor. */
TorSocket::TorSocket(QHostAddress socksAddr, quint16 socksPort, QObject *parent)
: QTcpSocket(parent),
  _socksAddr(socksAddr),
  _socksPort(socksPort)
{
}

/** Connects to the specified hostname and port via Tor. */
void
TorSocket::connectToHost(const QString &remoteHost, quint16 remotePort)
{
  /* Connect to the local socks proxy. */
  QTcpSocket::connectToHost(_socksAddr, _socksPort);
  
  /* Wait for the local connection. */
  if (waitForConnected(LOCAL_CONNECT_TIMEOUT)) {
    /* We're connected to the proxy, so send our part of the handshake. */
    QObject::connect(this, SIGNAL(readyRead()),
                     this, SLOT(onHandshakeResponse()));
    sendSocksHandshake(remoteHost, remotePort);
  }
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
TorSocket::sendSocksHandshake(const QString &remoteHost, quint16 remotePort)
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
TorSocket::onHandshakeResponse()
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
      /* Connection status was okay. */
      emit connected();
    } else {
      /* Remote connection failed, so close the connection to the proxy. */
      disconnectFromHost();
    }
  }
}
