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
** \file geoipcache.h
** \version $Id$
** \brief Caches the results of previous GeoIP requests
*/

#ifndef _GEOIPCACHE_H
#define _GEOIPCACHE_H

#include <QString>
#include <QHash>
#include <QHostAddress>

#include "geoipcacheitem.h"


class GeoIpCache
{
public:
  /** Default constructor. */
  GeoIpCache();
  
  /** Writes the current cache to disk. */
  bool saveToDisk(QString *errmsg = 0);
  /** Reads the cache in from disk. */
  bool loadFromDisk(QString *errmsg = 0);
  
  /** Returns the location currently used for the cache file. */
  QString cacheFilename();
  /** Caches the given IP and geographic information to disk. */
  void cache(GeoIp geoip);
  /** Returns a GeoIp object for the given IP from cache. */
  GeoIp geoip(QHostAddress ip);
  /** Returns true if the given IP address is cached. */
  bool contains(QHostAddress ip);
  
private:
  QHash<quint32, GeoIpCacheItem> _cache;  /**< List of cached GeoIp objects. */  
};

#endif

