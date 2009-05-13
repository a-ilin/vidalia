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

#include "GeoIpCache.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QHostAddress>

class GeoIp;
class GeoIpRequest;
class GeoIpResponse;
class QString;
class QAbstractSocket;


class GeoIpResolver : public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  GeoIpResolver(QObject *parent = 0);
  
  /** Sets the address and port of Tor, through which GeoIP requests will be
   * made. */
  void setSocksHost(const QHostAddress &addr, quint16 port);
  /** Resolves a single IP to a geographic location. */
  int resolve(const QHostAddress &ip);
  /** Resolves a list of IPs to a geographic location. */
  int resolve(const QList<QHostAddress> &ips);
  /** Resolves <b>ip</b> to geographic information only if it is cached. */
  bool resolveFromCache(const QHostAddress &ip);
  /** Resolves a list of IPs to a geographic location, but only those which
   * are cached. Returns a list of which IPs were not cached. */
  QList<QHostAddress> resolveFromCache(const QList<QHostAddress> &ips);

signals:
  /** Emitted when a list of IPs have been resolved to lat/long. */
  void resolved(int id, const QList<GeoIp> &geoips);
  /** Emitted when a resolve has failed. */
  void resolveFailed(int id, const QString &errorString);

private slots:
  /** Called when the socket has connected to the Geo IP host. */
  void connected();
  /** Called when the socket has disconnected from the Geo IP host. */
  void disconnected();
  /** Called when an error has occurred getting the Geo IP information. */
  void socketError(const QString &errorString);

private:
  /** Creates an HTTP request for Geo IP information. */
  GeoIpRequest* createRequest(const QList<QHostAddress> &ips);

  void parseGeoIpResponse(const QByteArray &response, GeoIpRequest *request);

  /**< Cached GeoIp objects. */
  GeoIpCache*  _cache;
  /**< List of sockets used for requests. */
  QHash<QAbstractSocket *,GeoIpRequest*> _requestList;
  /** Tor's SocksListenAddress. */
  QHostAddress _socksAddr;
  /** Tor's SocksPort. */
  quint16 _socksPort;
};

#endif

