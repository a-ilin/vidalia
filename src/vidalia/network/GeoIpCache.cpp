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
** \file GeoIpCache.cpp
** \version $Id$
** \brief Caches the results of previous GeoIP requests
*/

#include "GeoIpCache.h"
#include "GeoIpCacheItem.h"
#include "GeoIp.h"
#include "Vidalia.h"

#include "file.h"
#include "stringutil.h"

#include <QFile>
#include <QDir>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QHostAddress>


GeoIpCache::GeoIpCache(QObject *parent)
  : QObject(parent)
{
  loadFromDisk();
}

QString
GeoIpCache::cacheFileName() const
{
  return (Vidalia::dataDirectory() + "/geoip-cache");
}

bool
GeoIpCache::saveToDisk(QString *errmsg)
{
  /* Make sure we have a data directory. */
  if (!create_path(Vidalia::dataDirectory())) {
    return false;
  }
  
  /* Try to open a temporary cache file for writing */
  QFile tmpCacheFile(cacheFileName() + ".tmp");
  if (!tmpCacheFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return err(errmsg, tmpCacheFile.errorString());
  }

  /* Write the cache entries to the file. */
  QTextStream cache(&tmpCacheFile);
  foreach (GeoIpCacheItem cacheItem, _cache) {
    /* Save the cache item if it's not too old. */
    if (!cacheItem.isExpired()) {
      cache << cacheItem.toCacheString() << endl;
    }
  }

  QFile cacheFile(cacheFileName());
  /* Check if an previous cache file exists. */
  if (cacheFile.exists()) {
    /* A previous cache file exists, so try to remove it */
    if (!cacheFile.remove()) {
      return err(errmsg, cacheFile.errorString());
    }
  }
  /* Rename the temporary file into place. */
  if (!tmpCacheFile.rename(cacheFile.fileName())) {
    return err(errmsg, tmpCacheFile.errorString());
  }
  return true;
}

bool
GeoIpCache::loadFromDisk(QString *errmsg)
{
  QFile cacheFile(cacheFileName());

  if (cacheFile.exists()) {
    /* Try to open the cache file */
    if (!cacheFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return err(errmsg, cacheFile.errorString());
    }
    
    /* Read the cached items from the cache file */
    QTextStream cache(&cacheFile);
    QString line = cache.readLine();
    while (! line.isNull()) {
      /* Create a GeoIpCacheItem from the line and save it */
      GeoIpCacheItem item = GeoIpCacheItem::fromCacheString(line);
      if (item.isValid() && ! item.isExpired())
        addToCache(item);

      line = cache.readLine();
    }
    vInfo("Parsed %1 GeoIP entries from '%2'").arg(_cache.size())
                                              .arg(cacheFileName());
  }
  return true;
}

void
GeoIpCache::addToCache(const GeoIp &geoip)
{
  /* Create a "range" consisting of only a single IP address. */
  if (! contains(geoip.ip()))
    addToCache(geoip.ip(), geoip.ip(), geoip);
}

void
GeoIpCache::addToCache(const QHostAddress &from, const QHostAddress &to,
                       const GeoIp &geoip)
{
  /* New cache entries expire 30 days from the time they are first cached. */
  QDateTime expires = QDateTime::currentDateTime().toUTC().addDays(30);
  
  /* Create a new GeoIpCacheItem and add it to the cache */
  addToCache(GeoIpCacheItem(from, to, geoip, expires));
}

void
GeoIpCache::addToCache(const GeoIpCacheItem &ci)
{
  if (! ci.isValid() || ci.isExpired())
    return;

  /* The key for the cache is the last IP address in the range of IP addresses
   * covered by the GeoIpCacheItem. We do this because QMap::upperBound() and 
   * QMap::lowerBound() return an iterator to the next item higher than the
   * search value (an IP address) if an exact match is not found.
   */
  _cache.insert(ci.ipRangeEnd().toIPv4Address(), ci);
}

GeoIp
GeoIpCache::geoIpForAddress(const QHostAddress &ip)
{
  GeoIpCacheMap::iterator i = _cache.upperBound(ip.toIPv4Address());
  if (i != _cache.end() && (*i).contains(ip)) 
    return (*i).toGeoIp(ip);
  return GeoIp();
}

bool
GeoIpCache::contains(const QHostAddress &ip)
{
  GeoIpCacheMap::iterator i = _cache.upperBound(ip.toIPv4Address());
  return (i != _cache.end() && (*i).contains(ip));
}


