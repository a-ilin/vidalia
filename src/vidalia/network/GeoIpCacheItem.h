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

#include "GeoIp.h"

#include <QDateTime>


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
  /** Returns true if this cache item is empty and invalid. */
  bool isEmpty() const;

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

