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
G** \file GeoIpResolver.h
** \version $Id$
** \brief Requests GeoIP information and caches the result
*/

#ifndef _GEOIPRESOLVER_H
#define _GEOIPRESOLVER_H

#include "GeoIp.h"
#include "GeoIpCache.h"
#include "GeoIpRequest.h"
#include "GeoIpResponse.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>
#include <QHostAddress>


class GeoIpResolver : public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  GeoIpResolver();
  
  /** Sets the address and port of Tor, through which GeoIP requests will be
   * made. */
  void setSocksHost(QHostAddress addr, quint16 port);
  /** Resolves a single IP to a geographic location. */
  int resolve(QHostAddress ip);
  /** Resolves a list of IPs to a geographic location. */
  int resolve(QList<QHostAddress> ips);
  /** Resolves <b>ip</b> to geographic information only if it is cached. */
  bool resolveFromCache(QHostAddress ip);
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

  /**< Cached GeoIp objects. */
  GeoIpCache  _cache;
  /**< List of sockets used for requests. */
  QHash<QAbstractSocket *,GeoIpRequest*> _requestList;
  /** Tor's SocksListenAddress. */
  QHostAddress _socksAddr;
  /** Tor's SocksPort. */
  quint16 _socksPort;
};

#endif

