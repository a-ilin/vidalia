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
** \file GeoIpCacheItem.h
** \version $Id$
** \brief Cached result of a single IP-to-geolocation result
*/

#ifndef _GEOIPCACHEITEM_H
#define _GEOIPCACHEITEM_H

#include <QHash>
#include <QString>
#include <QVariant>
#include <QDateTime>

class GeoIp;
class QHostAddress;


class GeoIpCacheItem
{
public:
  /** Default constructor
   */
  GeoIpCacheItem();

  /** Constructor.
   */
  GeoIpCacheItem(const QHostAddress &from, const QHostAddress &to,
                 const GeoIp &geoIp, const QDateTime &expires);

  /** Returns the first IP address in the range of IP addresses associated
   * with this GeoIpCacheItem.
   */
  QHostAddress ipRangeStart() const;

  /** Returns the last IP address in the range of IP addresses associated
   * with this GeoIpCacheItem.
   */
  QHostAddress ipRangeEnd() const;

  /** Returns true if <b>ip</b> is within the range of IP addresses associated
   * with this GeoIpCacheItem.
   * \sa ipRangeStart()
   * \sa ipRangeEnd()
   */
  bool contains(const QHostAddress &ip) const;

  /** Returns true if this GeoIpCacheItem object contains valid values for
   * all required fields.
   */
  bool isValid() const;

  /** Returns true if the cache item is too old to be considered accurate.
   * Cached GeoIp responses are considered valid for thirty days after they
   * are first added to the cache.
   */
  bool isExpired() const;

  /** Returns a GeoIp object for <b>ip</b>, populated with the cached
   * geographic information stored by this GeoIpCacheObject. If <b>ip</b>
   * is not within the range of IP addresses associated with this object,
   * an empty GeoIp object is returned.
   * \sa contains
   */
  GeoIp toGeoIp(const QHostAddress &ip) const;

  /** Formats the fields contained in this GeoIpCacheItem as a string
   * suitable for writing to a cache file.
   */
  QString toCacheString() const;
  
  /** Parses <b>cacheLine</b> and constructs a new GeoIpCacheItem object
   * with the parsed values. The format of <b>cacheLine</b> must follow the
   * format as produced by toCacheString().
   * \sa toCacheString()
   */
  static GeoIpCacheItem fromCacheString(const QString &cacheLine);

private:
  quint32 _fromIp;
  quint32 _toIp;
  QDateTime _expires;  /**< Time this item was cached. */
  QHash<QString,QVariant> _fields;
};

#endif

