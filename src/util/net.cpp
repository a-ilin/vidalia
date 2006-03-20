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
 * \file net.cpp
 * \version $Id$
 */

#include <QHostInfo>
#include <QList>
#include <QUrl>

#include "net.h"
#include "http.h"


/** Returns the IP address of the local machine. */
QHostAddress
net_local_address()
{
  QHostInfo localInfo = QHostInfo::fromName(QHostInfo::localHostName());
  if (localInfo.error() == QHostInfo::NoError) {
    foreach (QHostAddress addr, localInfo.addresses()) {
      if (addr.protocol() == QAbstractSocket::IPv4Protocol &&
          !(addr == QHostAddress::LocalHost)) {
        return addr;
      }
    }
  }
  return QHostAddress::LocalHost;
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

/** Returns true if the given string representation of an IP 
 * address is valid. */
bool
net_is_valid_ip(QString ip)
{
  return QHostAddress().setAddress(ip);
}

/** Returns a pre-defined, static list of servers whom we can ask for our 
 * public IP address. */
QList<QUrl>
get_check_ip_sites()
{
  static QList<QUrl> sites;
  if (sites.count() == 0) {
    sites.append(QUrl("http://ipid.shat.net/iponly/"));
    sites.append(QUrl("http://freehaven.net/~edmanm/ip.php"));
    sites.append(QUrl("http://vidalia-project.net/iptest/ip.php"));
  } else {
    sites.append(sites.takeFirst());
  }
  return sites;
}

/** Asks a pre-defined list of servers what they think this machine's public
 * IP address is.
 * \param ip Stores the ip after a successful request
 * \return false if none of the requests were successful
 */
bool
net_get_public_ip(QString &ip)
{
  QList<QUrl> sites = get_check_ip_sites();
  foreach (QUrl site, sites) {
    Http http(site.host());
    if (http.request(site.path())) {
      if (net_is_valid_ip(http.body())) {
        ip = http.body();
        return true;
      }
    }
  }
  return false;
}

