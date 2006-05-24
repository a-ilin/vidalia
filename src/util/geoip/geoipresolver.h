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
 */

#ifndef _GEOIPRESOLVER_H
#define _GEOIPRESOLVER_H

#include <QList>
#include <QHash>
#include <QHttp>
#include <QString>
#include <QTcpSocket>
#include <QByteArray>
#include <QHostAddress>

#include "geoip.h"
#include "geoipcache.h"


class GeoIpResolver : public QHttp
{
  Q_OBJECT

public:
  /** Default constructor. */
  GeoIpResolver();

  /** Resolves a single IP to a geographic location. */
  int resolve(QHostAddress ip);
  /** Resolves a list of IPs to a geographic location. */
  int resolve(QList<QHostAddress> ips);
  /** Resolves a list of IPs to a geographic location, but only those which
   * are cached. Returns a list of which IPs were not cached. */
  QList<QHostAddress> resolveFromCache(QList<QHostAddress> ips);

signals:
  /** Emitted when a single IP has been resolved to lat/long. */
  void resolved(GeoIp geoip);
  /** Emitted when a list of IPs have been resolved to lat/long. */
  void resolved(QList<GeoIp> geoips);
  /** Emitted when a resolve has failed. */
  void resolveFailed(int id, QString errorString);

private slots:
  /** Called when a request has been completed. */
  void requestFinished(int id, bool error);
  
private:
  /** Creates a request for the given list of IP addresses. */
  QByteArray buildRequest(QList<QHostAddress> ip);
  /** Parses the response from a request for GeoIp information. */
  QList<GeoIp> parseResponse(QString response);

  GeoIpCache  _cache; /**< Cached GeoIp objects. */
  QHash<int,QTcpSocket *> _requestList; /**< List of sockets used for requests. */
};

#endif

