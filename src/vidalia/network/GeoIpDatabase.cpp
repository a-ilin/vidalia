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
** \file GeoIpDatabase.cpp
** \brief Interface to a local MaxMind GeoIP database
*/

#include "GeoIpDatabase.h"
#include "GeoIpRecord.h"
#include "Vidalia.h"

#include <QString>
#include <QHostAddress>


/** Default constructor. */
GeoIpDatabase::GeoIpDatabase(QObject *parent)
  : QObject(parent), _db(0)
{
}

GeoIpDatabase::~GeoIpDatabase()
{
  close();
}

bool
GeoIpDatabase::open(const QString &fname)
{
  if (isOpen())
    close();

  _db = GeoIP_open(fname.toLocal8Bit().constData(), GEOIP_STANDARD);
  if (_db) {
    GeoIP_set_charset(_db, GEOIP_CHARSET_UTF8);
    return true;
  }
  vError("Unable to open local GeoIP database: %1").arg(fname);
  return false;
}

void
GeoIpDatabase::close()
{
  if (isOpen()) {
    GeoIP_delete(_db);
    _db = 0;
  }
}

bool
GeoIpDatabase::isOpen() const
{
  return (_db != 0);
}

GeoIpDatabase::DatabaseType
GeoIpDatabase::type() const
{
  if (! isOpen())
    return UnknownDatabase;

  switch (_db->databaseType) {
    case GEOIP_COUNTRY_EDITION:
    case GEOIP_COUNTRY_EDITION_V6:
      return CountryDatabase;

    case GEOIP_CITY_EDITION_REV0:
    case GEOIP_CITY_EDITION_REV1:
      return CityDatabase;

    case GEOIP_REGION_EDITION_REV0:
    case GEOIP_REGION_EDITION_REV1:
      return RegionDatabase;

    case GEOIP_ORG_EDITION:
      return OrganizationDatabase;

    case GEOIP_ISP_EDITION:
      return IspDatabase;

    case GEOIP_PROXY_EDITION:
      return ProxyDatabase;

    case GEOIP_ASNUM_EDITION:
      return AsnDatabase;

    case GEOIP_NETSPEED_EDITION:
      return NetSpeedDatabase;

    case GEOIP_DOMAIN_EDITION:
      return DomainDatabase;

    default:
      return UnknownDatabase;
  }
}

QString
GeoIpDatabase::countryCodeByAddr(const QHostAddress &ip)
{
  if (isOpen() && ! ip.isNull()) {
    const char *addr = ip.toString().toAscii().constData();
    const char *countryCode = GeoIP_country_code_by_addr(_db, addr);
    if (countryCode)
      return QString::fromUtf8(countryCode);
  }
  return QString();
}

GeoIpRecord
GeoIpDatabase::recordByAddr(const QHostAddress &ip)
{
  if (isOpen() && ! ip.isNull()) {
    const char *addr = ip.toString().toAscii().constData();

    GeoIPRecord *r;
    if (ip.protocol() == QAbstractSocket::IPv6Protocol)
      r = GeoIP_record_by_addr_v6(_db, addr);
    else
      r = GeoIP_record_by_addr(_db, addr);

    if (r) {
      QString countryCode = QString::fromUtf8(r->country_code);
      QString countryName = QString::fromUtf8(r->country_name);
      QString city = QString::fromUtf8(r->city);

      QString region;
      const char *regionName = GeoIP_region_name_by_code(r->country_code,
                                                         r->region);
      if (regionName)
        region = QString::fromUtf8(regionName);

      return GeoIpRecord(ip, r->latitude, r->longitude, city, region,
                         countryName, countryCode);
    }
  }
  return GeoIpRecord();
}

