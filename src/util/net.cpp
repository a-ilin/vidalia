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

#include "net.h"

/** Returns the IP address of the local machine. */
QHostAddress
net_local_address()
{
  QAbstractSocket localSock(QAbstractSocket::TcpSocket, 0);
  QHostAddress localAddr = localSock.localAddress();
  if (localAddr.isNull()) {
    return QHostAddress::LocalHost;
  }
  return localAddr;
}

/** Returns true if the given address is a private IP address. */
bool
net_is_private_address(QHostAddress addr)
{
  quint32 ip = addr.toIPv4Address();
  /* Based on RFC1918 and Tor's util.c */
  return (((ip & 0xFF000000) == 0x00000000) || /*        0/8 */
          ((ip & 0xFF000000) == 0x0A000000) || /*       10/8 */
          ((ip & 0xFF000000) == 0x7F000000) || /*      127/8 */
          ((ip & 0xFFF00000) == 0xAC100000) || /*  172.16/12 */
          ((ip & 0xFFFF0000) == 0xA9FE0000) || /* 169.254/16 */
          ((ip & 0xFFFF0000) == 0xC0A80000));  /* 192.168/16 */
}

