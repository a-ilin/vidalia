/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file GeoIpResolver.cpp
** \brief Retrieves GeoIP information either from Tor or from a local
** database and returns the result.
*/

#include "GeoIpResolver.h"
#include "GeoIpRecord.h"
#include "Vidalia.h"


/** Default constructor. */
GeoIpResolver::GeoIpResolver(QObject *parent)
  : QObject(parent), _useLocalDatabase(false)
{
}

bool
GeoIpResolver::setLocalDatabase(const QString &databaseFile)
{
#if defined(USE_GEOIP)
  return _database.open(databaseFile);
#else
  return false;
#endif
}

void
GeoIpResolver::setUseLocalDatabase(bool useLocalDatabase)
{
  _useLocalDatabase = useLocalDatabase;
}

GeoIpRecord
GeoIpResolver::resolveUsingTor(const QHostAddress &ip)
{
  QString countryCode = Vidalia::torControl()->ipToCountry(ip);
  if (! countryCode.isEmpty()) {
    QPair<float,float> coords = CountryInfo::countryLocation(countryCode);
    return GeoIpRecord(ip, coords.first, coords.second,
                       CountryInfo::countryName(countryCode),
                       countryCode);
  }
  return GeoIpRecord();
}

GeoIpRecord
GeoIpResolver::resolveUsingLocalDatabase(const QHostAddress &ip)
{
#if defined(USE_GEOIP)
  if (_database.type() == GeoIpDatabase::CityDatabase) {
    return _database.recordByAddr(ip);
  } else {
    QString countryCode = _database.countryCodeByAddr(ip);
    if (! countryCode.isEmpty()) {
      QPair<float,float> coords = CountryInfo::countryLocation(countryCode);
      return GeoIpRecord(ip, coords.first, coords.second,
                         CountryInfo::countryName(countryCode),
                         countryCode);
    }
  }
#endif
  return GeoIpRecord();
}

/** Resolves a single IP to a geographic location. */
GeoIpRecord
GeoIpResolver::resolve(const QHostAddress &ip)
{
#if defined(USE_GEOIP)
  if (_useLocalDatabase)
    return resolveUsingLocalDatabase(ip);
#endif
  return resolveUsingTor(ip);
}

