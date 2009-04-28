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
#include "Vidalia.h"

#include "file.h"
#include "stringutil.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

/* Location of Vidalia's geoip cache file. Qt docs claims that QFile will
 * translate the "/" correctly on Windows. Hopefully they didn't lie. */
#define CACHE_FILENAME  (Vidalia::dataDirectory() + "/geoip-cache")


/** Constructor. */
GeoIpCache::GeoIpCache()
{
  loadFromDisk();
}

/** Returns the location currently used for the cache file. */
QString
GeoIpCache::cacheFilename()
{
  return CACHE_FILENAME;
}

/** Writes the current cache to disk. */
bool
GeoIpCache::saveToDisk(QString *errmsg)
{
  /* Make sure we have a data directory. */
  if (!create_path(Vidalia::dataDirectory())) {
    return false;
  }
  
  /* Try to open a temporary cache file for writing */
  QFile tmpCacheFile(CACHE_FILENAME + ".tmp");
  if (!tmpCacheFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return err(errmsg, tmpCacheFile.errorString());
  }
  
  /* Write the cache entries to the file. */
  QTextStream cache(&tmpCacheFile);
  foreach (GeoIpCacheItem cacheItem, _cache.values()) {
    /* Save the cache item if it's not too old. */
    if (!cacheItem.isExpired()) {
      cache << cacheItem.toString() << endl;
    }
  }
  
  QFile cacheFile(CACHE_FILENAME);
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

/** Reads the cache contents in from disk. This function returns true if no
 * cache file exists, since it's possible nothing has been cached yet. */
bool
GeoIpCache::loadFromDisk(QString *errmsg)
{
  QFile cacheFile(CACHE_FILENAME);
  
  if (cacheFile.exists()) {
    /* Try to open the cache file */
    if (!cacheFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return err(errmsg, cacheFile.errorString());
    }
    
    /* Read the cached items from the cache file */
    QTextStream cache(&cacheFile);
    QString line = cache.readLine();
    while (!line.isNull()) {
      /* Create a GeoIpCacheItem from the line and save it */
      GeoIpCacheItem item = GeoIpCacheItem::fromString(line);
      if (!item.isEmpty() && !item.isExpired()) {
        /* Only load non-stale cache items. */
        _cache.insert(item.ip().toIPv4Address(), item);
      }
      line = cache.readLine();
    }
  }
  return true;
}

/** Caches the given IP and geographic information to disk. Call saveToDisk()
 * when you want to write the cache to disk. */
void
GeoIpCache::cache(GeoIp geoip)
{
  /* Store the entry in our in-memory cache */
  _cache.insert(geoip.ip().toIPv4Address(), 
                GeoIpCacheItem(geoip,QDateTime::currentDateTime()));
}

/** Returns a GeoIp object for the given IP from cache. */
GeoIp
GeoIpCache::geoip(QHostAddress ip)
{
  if (this->contains(ip)) {
    return _cache.value(ip.toIPv4Address()).geoip();
  }
  return GeoIp();
}

/** Returns true if the given IP address is cached and the cached information
 * is not stale. */
bool
GeoIpCache::contains(QHostAddress ip)
{
  quint32 ipv4 = ip.toIPv4Address();
  if (_cache.contains(ipv4)) {
    GeoIpCacheItem cacheItem = _cache.value(ipv4);
    return !cacheItem.isExpired();
  }
  return false;
}

