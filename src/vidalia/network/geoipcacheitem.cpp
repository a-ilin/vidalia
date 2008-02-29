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
** \file geoipcacheitem.cpp
** \version $Id$
** \brief Cached result of a single IP-to-geolocation result
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
 *                     <Geo IP Data>[:<Timestamp>]
 *
 * If no value for Timestamp is given, the current date and time will be used.
 * If the string cannot be parsed for valid cached GeoIP data, then an empty
 * GeoIpCacheItem object is returned. The calling method should call isEmpty()
 * on the returned GeoIpCacheItem object to ensure it got a valid object.
 */
GeoIpCacheItem
GeoIpCacheItem::fromString(QString cacheString)
{
  QDateTime timestamp;
  QStringList cacheData = cacheString.split(":");

  if (cacheData.size() >= 1) {
    GeoIp geoip = GeoIp::fromString(cacheData.at(0));
    if (cacheData.size() >= 2)
      timestamp.setTime_t(cacheData.at(1).toUInt());
    else
      timestamp = QDateTime::currentDateTime();
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

