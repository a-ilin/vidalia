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
 */

#include <QStringList>

#include "geoipcacheitem.h"


/** Constructor */
GeoIpCacheItem::GeoIpCacheItem(GeoIp geoip, QDateTime timestamp)
{
  _geoip = geoip;
  _timestamp = timestamp;
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
 */
GeoIpCacheItem
GeoIpCacheItem::fromString(QString cacheString)
{
  QDateTime timestamp;
  QStringList cacheData = cacheString.split(":");
  
  GeoIp geoip = GeoIp::fromString(cacheData.at(0));
  timestamp.setTime_t(cacheData.at(1).toUInt());
  return GeoIpCacheItem(geoip, timestamp);
}

/** Returns true if the cache item is too old to be considered valid. */
bool
GeoIpCacheItem::isExpired() const
{
  /* Consider cache entries to be valid for one month. */
  return (_timestamp.addMonths(1) < QDateTime::currentDateTime()); 
}

