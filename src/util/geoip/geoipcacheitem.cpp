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
 * \file geoipcacheitem.cpp
 * \version $Id$
 * \brief Cached result of a single IP-to-geolocation result
 */

#include <QStringList>

#include "geoipcacheitem.h"


/** Constructor */
GeoIpCacheItem::GeoIpCacheItem(GeoIp geoip, QDateTime timestamp)
{
  _geoip = geoip;
  _timestamp = timestamp;
}

/** Returns true if this cache item is empty and invalid. A valid cache item
 * must have a valid GeoIp object and timestamp. */
bool
GeoIpCacheItem::isEmpty() const
{
  return (_geoip.isEmpty() || _timestamp.isNull());
}

/** Returns a string representing the contents of this cache item, suitable
 * for writing to disk. The format is as in the following example:
 *                     <Geo IP Data>:<Timestamp>
 */
QString
GeoIpCacheItem::toString() const
{
  return _geoip.toString() + ":" + QString::number(_timestamp.toTime_t());
}

/** Returns a GeoIpCacheItem from a string as read from the cache that was
 * written to disk. The format is:
 *                     <Geo IP Data>:<Timestamp>
 *
 * If the string cannot be parsed for valid cached GeoIP data, then an empty
 * GeoIpCacheItem object is returned. The calling method should call isEmpty()
 * on the returned GeoIpCacheItem object to ensure it got a valid object.
 */
GeoIpCacheItem
GeoIpCacheItem::fromString(QString cacheString)
{
  QDateTime timestamp;
  QStringList cacheData = cacheString.split(":");
 
  if (cacheData.size() >= 2) {
    GeoIp geoip = GeoIp::fromString(cacheData.at(0));
    timestamp.setTime_t(cacheData.at(1).toUInt());
    return GeoIpCacheItem(geoip, timestamp);
  }
  return GeoIpCacheItem();
}

/** Returns true if the cache item is too old to be considered valid. Normal
 * cached responses are valid for one month. Cached UNKNOWN responses are
 * considered valid for one week. */
bool
GeoIpCacheItem::isExpired() const
{
  if (_geoip.isUnknown()) {
    return (_timestamp.addDays(7) < QDateTime::currentDateTime());
  }
  return (_timestamp.addMonths(1) < QDateTime::currentDateTime()); 
}

