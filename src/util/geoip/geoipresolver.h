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
 * \file geoipresolver.h
 * \version $Id$
 * \brief Requests GeoIP information and caches the result
 */

#ifndef _GEOIPRESOLVER_H
#define _GEOIPRESOLVER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>
#include <QHostAddress>
#include <util/torsocket.h>

#include "geoip.h"
#include "geoipcache.h"
#include "geoiprequest.h"
#include "geoipresponse.h"


class GeoIpResolver : public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  GeoIpResolver() {}

  /** Resolves a single IP to a geographic location. */
  int resolve(QHostAddress ip);
  /** Resolves a list of IPs to a geographic location. */
  int resolve(QList<QHostAddress> ips);
  /** Resolves a list of IPs to a geographic location, but only those which
   * are cached. Returns a list of which IPs were not cached. */
  QList<QHostAddress> resolveFromCache(QList<QHostAddress> ips);

signals:
  /** Emitted when a list of IPs have been resolved to lat/long. */
  void resolved(int id, QList<GeoIp> geoips);
  /** Emitted when a resolve has failed. */
  void resolveFailed(int id, QString errorString);

private slots:
  /** Called when the socket has connected to the Geo IP host. */
  void connected();
  /** Called when the socket has disconnected from the Geo IP host. */
  void disconnected();
  /** Called when an error has occurred getting the Geo IP information. */
  void socketError(QString errorString);

private:
  /** Creates an HTTP request for Geo IP information. */
  GeoIpRequest* createRequest(QList<QHostAddress> ips);
  /** Creates a socket used to request Geo IP information over Tor. */
  TorSocket* createRequestSocket();
  
  /**< Cached GeoIp objects. */
  GeoIpCache  _cache;
  /**< List of sockets used for requests. */
  QHash<TorSocket *,GeoIpRequest*> _requestList;
};

#endif

