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
** \file GeoIpCache.h
** \version $Id$
** \brief Caches the results of previous GeoIP requests
*/

#ifndef _GEOIPCACHE_H
#define _GEOIPCACHE_H

#include "GeoIpCacheItem.h"

#include <QObject>
#include <QMap>

class GeoIp;
class QString;
class QHostAddress;

typedef QMap<quint32, GeoIpCacheItem> GeoIpCacheMap;


class GeoIpCache : public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  GeoIpCache(QObject *parent = 0);
  
  /** Writes the current cache to disk. Returns true if the cache file was
   * successfully saved to disk. Otherwise, returns false and sets
   * <b>errmsg</b> to a string describing the error encountered, if
   * <b>errmsg</b> is not null.
   */
  bool saveToDisk(QString *errmsg = 0);
  
  /** Reads the cache in from disk. Returns true if the cache file was
   * successfully read. Otherwise, returns false and sets <b>errmsg</b> to
   * a string describing the error encountered, if <b>errmsg</b> is not null.
   */
  bool loadFromDisk(QString *errmsg = 0);
  
  /** Returns the location currently used for the cache file.
   */
  QString cacheFileName() const;

  /** Caches the geographic information in <b>geoip</b> associated with a
   * single IP address.
   */
  void addToCache(const GeoIp &geoip);

  /** Caches the geographic information in <b>geoip</b> associated with a
   * range of IP addresses, from <b>from</b> to <b>to</b> (inclusive).
   */
  void addToCache(const QHostAddress &from, const QHostAddress &to,
                  const GeoIp &geoip);

  /** Returns a GeoIp object for the given <b>ip</b> from cache. If no cached
   * information is known for <b>ip</b>, an empty GeoIp object is returned.
   */
  GeoIp geoIpForAddress(const QHostAddress &ip);

  /** Returns true if the cache contains geographic location information for
   * <b>ip</b>. Otherwise, returns false.
   */
  bool contains(const QHostAddress &ip);

private:
  /** Adds the GeoIpCacheItem <b>ci</b> to the cache. */
  void addToCache(const GeoIpCacheItem &ci);

  /**< List of cached GeoIp objects. */
  GeoIpCacheMap _cache;  
};

#endif

