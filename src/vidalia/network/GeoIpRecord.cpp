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
** \file GeoIpRecord.cpp
** \brief Associates an IP with a geographic location
*/

#include "GeoIpRecord.h"

#include <QStringList>

/** Verifies a latitude is between -90.0 and 90.0 degrees. */
#define IS_VALID_LATITUDE(x)    (((x) >= -90.0) && ((x) <= 90.0))
/** Verifies a longitude is between -180.0 and 180.0 degrees. */
#define IS_VALID_LONGITUDE(x)   (((x) >= -180.0) && ((x) <= 180.0))


GeoIpRecord::GeoIpRecord()
{
  _latitude  = 0.0;
  _longitude = 0.0;
}

GeoIpRecord::GeoIpRecord(const QHostAddress &ip, float latitude, float longitude,
                         const QString &country, const QString &countryCode)
{
  _ip = ip;
  _latitude = latitude;
  _longitude = longitude;
  _country  = country;
  _countryCode = countryCode;
}

GeoIpRecord::GeoIpRecord(const QHostAddress &ip, float latitude, float longitude,
                         const QString &city, const QString &region,
                         const QString &country, const QString &countryCode)
{
  _ip = ip;
  _latitude = latitude;
  _longitude = longitude;
  _city = city;
  _region = region;
  _country = country;
  _countryCode = countryCode;
}

bool
GeoIpRecord::isValid() const
{
  return (! _ip.isNull()
            && IS_VALID_LATITUDE(_latitude)
            && IS_VALID_LONGITUDE(_longitude));
}

QString
GeoIpRecord::toString() const
{
  QStringList location;

  /* Add the city name (if present) */
  if (!_city.isEmpty())
    location << _city;

  /* Add the full state or region name (if present) */
  if (!_region.isEmpty() && _region != _city)
      location << _region;

  /* Add the country name or the country code (if present) */
  if (!_country.isEmpty())
    location << _country;
  else if (!_countryCode.isEmpty())
    location << _countryCode;

  return location.join(", ");
}

