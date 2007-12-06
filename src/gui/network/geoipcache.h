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
 * \file geoipcache.h
 * \version $Id$
 * \brief Caches the results of previous GeoIP requests
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

