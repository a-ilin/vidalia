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
 * \file geoipcacheitem.h
 * \version $Id$
 * \brief Cached result of a single IP-to-geolocation result
 */

#ifndef _GEOIPCACHEITEM_H
#define _GEOIPCACHEITEM_H

#include <QDateTime>

#include "geoip.h"


class GeoIpCacheItem
{
public:
  /** Default constructor */
  GeoIpCacheItem() {};
  /** Constructor. */
  GeoIpCacheItem(GeoIp geoip, QDateTime timestamp);

  /** Returns the IP of this cache item. */
  QHostAddress ip() const { return _geoip.ip(); }
  /** Returns the cached GeoIp object. */
  GeoIp geoip() const { return _geoip; }
  /** Returns true if this cache item is expired. */
  bool isExpired() const;
 
  /** Returns a string representing the contents of this cache item, suitable
   * for writing to disk. */
  QString toString() const;
  /** Returns a GeoIpCacheItem from a string as read from the cache that was
   * written to disk. */
  static GeoIpCacheItem fromString(QString cacheString);

private:
  GeoIp     _geoip;      /**< Cached GeoIp item. */
  QDateTime _timestamp;  /**< Time this item was cached. */
};

#endif

