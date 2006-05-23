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
 *
 *    128.213.48.13,Troy,NY,US,42.7495,-73.5951,1138402852
 */
QString
GeoIpCacheItem::toString() const
{
  QString s;
  s.append(_geoip.ip().toString());
  s.append("," + _geoip.city());
  s.append("," + _geoip.state());
  s.append("," + _geoip.country());
  s.append("," + QString::number(_geoip.latitude(),  'f', 4));
  s.append("," + QString::number(_geoip.longitude(), 'f', 4));
  s.append("," + QString::number(_timestamp.toTime_t()));
  return s;
}

/** Returns a GeoIpCacheItem from a string as read from the cache that was
 * written to disk. */
GeoIpCacheItem
GeoIpCacheItem::fromString(QString cacheString)
{
  QDateTime timestamp;
  QStringList cacheData = cacheString.split(",");
  
  QHostAddress   ip(cacheData.at(0));
  QString city    = cacheData.at(1);
  QString state   = cacheData.at(2);
  QString country = cacheData.at(3);
  float latitude  = cacheData.at(4).toFloat();
  float longitude = cacheData.at(5).toFloat();
  timestamp.setTime_t(cacheData.at(6).toUInt());

  return GeoIpCacheItem(
           GeoIp(ip, latitude, longitude, city, state, country), 
           timestamp);
}

/** Returns true if the cache item is too old to be considered valid. */
bool
GeoIpCacheItem::isExpired() const
{
  /* Consider cache entries to be valid for one month. */
  return (_timestamp.addMonths(1) < QDateTime::currentDateTime()); 
}

