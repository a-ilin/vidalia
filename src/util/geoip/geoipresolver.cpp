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
 * \file geoipresolver.cpp
 * \version $Id$
 */

#include <util/torsocket.h>

#include "geoipresolver.h"

/** Host for the geo ip information. 
 * XXX we really need more than one host here, even with 
  *    smart caching. */
#define GEOIP_HOST    "geoip.vidalia-project.net"
/** Page that we request the geo ip information from. */
#define GEOIP_PAGE    "/geoip.pl"


/** Default constructor. */
GeoIpResolver::GeoIpResolver()
{
  /* Set the geo ip information host */
  setHost(GEOIP_HOST);
  
  /* We want to find out when our requests finish. */
  connect(this, SIGNAL(requestFinished(int,bool)),
          this,   SLOT(requestFinished(int,bool)));
}

/** Creates a request for the given list of IP addresses. */
QByteArray
GeoIpResolver::buildRequest(QList<QHostAddress> ips)
{
  int ipcount = ips.size();
  QString request = "ip=";

  /* Build a comma-delimited list of IPs whose geographic information we would
   * like to learn. */
  for (int i = 0; i < ipcount; i++) {
    request.append(ips.at(i).toString());
    if (i < ipcount-1) {
      request.append(",");
    }
  }
  return request.toAscii();
}

/** Parses the response from a request for GeoIp information. */
QList<GeoIp> 
GeoIpResolver::parseResponse(QString response)
{
  QList<GeoIp> geoips;
  int newCacheItems = 0;
  QStringList lines = response.split("\n");

  /* For each line in the response, parse its geoip information */
  foreach (QString line, lines) {
    GeoIp geoip = GeoIp::fromString(line);
    if (!geoip.isEmpty()) {
      _cache.cache(geoip);
      geoips << geoip;
      newCacheItems++;
    }
  }

  /* If there are any new cached items, save them to disk */
  if (newCacheItems > 0) {
    _cache.saveToDisk();
  }
  return geoips;
}

/** Resolves a list of IPs to a geographic location, but only those which are
 * cached. Returns a list of IPs that were not in the cache. */
QList<QHostAddress>
GeoIpResolver::resolveFromCache(QList<QHostAddress> ips)
{
  QList<GeoIp> cached;

  /* Build a list of which IPs have cached GeoIp information */
  foreach (QHostAddress ip, ips) {
    if (_cache.contains(ip)) {
      ips.removeAt(ips.indexOf(ip));
      cached << _cache.geoip(ip);
    }
  }

  /* If any were cached, emit their results */
  if (cached.size() > 0) {
    emit resolved(cached);
  }
  return ips;
}

/** Resolves a single IP to a geographic location. */
int
GeoIpResolver::resolve(QHostAddress ip)
{
  return resolve(QList<QHostAddress>() << ip);
}

/** Resolves a list of IPs to a geographic location. */
int
GeoIpResolver::resolve(QList<QHostAddress> ips)
{
  int requestId;
  
  /* Resolve the cached IPs and get a list of IPs that still need to be
   * resolved to a lat and long. */
  QList<QHostAddress> uncached = resolveFromCache(ips);
  if (!uncached.size()) {
    return -1;
  }

  /* Create a socket used ot request the geo ip information.
   * XXX replace this with a TorSocket to do the request through Tor. */
  QTcpSocket *socket = new QTcpSocket();
  setSocket(socket);
 
  /* Send the request and return the request ID */
  requestId = post(GEOIP_PAGE, buildRequest(uncached));
  _requestList.insert(requestId, socket);
  return requestId;
}

/** Called when a request has been completed. If no error occurred, then the
 * response will be parsed for IP geolocation information. */
void
GeoIpResolver::requestFinished(int id, bool error)
{
  /* Check if this is a request we are interested in */
  if (!_requestList.contains(id)) {
    return;
  }
  delete  _requestList.value(id);
 
  /* If the request failed, let callers know why */
  if (error) {
    emit resolveFailed(id, errorString());  
    return;
  }
  
  /* Parse the responses and emit the results (if any) */
  QByteArray response = readAll();
  QList<GeoIp> geoips = parseResponse(response);
  if (geoips.size() > 0) {
    emit resolved(geoips);
  }
}

