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
** \file GeoIp.cpp
** \version $Id$
** \brief Associates an IP with a geographic location
*/

#include "GeoIp.h"

#include <QStringList>

/** Verifies a latitude is between -90.0 and 90.0 degrees. */
#define IS_VALID_LATITUDE(x)    (((x) >= -90.0) && ((x) <= 90.0))
/** Verifies a longitude is between -180.0 and 180.0 degrees. */
#define IS_VALID_LONGITUDE(x)   (((x) >= -180.0) && ((x) <= 180.0))


/** Constructor */
GeoIp::GeoIp(QHostAddress ip)
{
  _ip = ip;
  _latitude = _longitude = 0xFFFF;
}

/** Constructor. */
GeoIp::GeoIp(QHostAddress ip, float latitude, float longitude, 
             QString city, QString state, QString country)
{
  _ip        = ip;
  _latitude  = latitude;
  _longitude = longitude;
  _city      = city;
  _state     = state;
  _country   = country;
}

/** Parses the GeoIp information from a comma-delimited string. The format of
 * the string is as in the following example:
 *
 *      128.213.48.13,Troy,NY,US,42.7495,-73.5951,1138402852
 */
GeoIp
GeoIp::fromString(QString geoip)
{
  /* Split comma-delimited data fields */
  QStringList data = geoip.split(",");
  
  if (data.size() == 2 && data.at(1).toLower() == "unknown") {
    return GeoIp(QHostAddress(data.at(0)));
  } else if (data.size() < 6) {
    return GeoIp();
  }
  
  /* Parse the data from the string */
  QHostAddress   ip(data.at(0));
  QString city    = data.at(1);
  QString state   = data.at(2);
  QString country = data.at(3);
  float latitude  = data.at(4).toFloat();
  float longitude = data.at(5).toFloat();
 
  /* Create a new GeoIp object with the parsed data. */
  return GeoIp(ip, latitude, longitude, city, state, country);
}

/** Formats the GeoIp information as a comma-delimited string. */
QString
GeoIp::toString() const
{
  QString s;
  /* Assemble and comma-delimit the data fields */
  s.append(_ip.toString());
  s.append("," + _city);
  s.append("," + _state);
  s.append("," + _country);
  s.append("," + QString::number(_latitude,  'f', 4));
  s.append("," + QString::number(_longitude, 'f', 4));
  return s;
}

/** Returns true if the GeoIp object is invalid. */
bool
GeoIp::isEmpty() const
{
  return (_ip.isNull() && 
          !IS_VALID_LATITUDE(_latitude) && 
          !IS_VALID_LONGITUDE(_longitude));
}

/** Returns true if the GeoIp object is valid, but no location information
 * is known for the associated IP address. */
bool
GeoIp::isUnknown() const
{
  return (!_ip.isNull() && 
          !IS_VALID_LATITUDE(_latitude) && 
          !IS_VALID_LONGITUDE(_longitude));
}

/** Returns a human-readable string of GeoIp location information. */
QString
GeoIp::toLocation() const
{
  QStringList location;
  
  /* Add the city name (if present) */
  if (!_city.isEmpty()) {
    location << _city;
  }
  /* Add the state or region name (if present) */
  if (!_state.isEmpty()) {
    /* Only display non-numeric region codes. */
    bool valid = true;
    for (int i = 0; i < _state.length(); i++) {
      if (_state[i].isDigit()) {
        valid = false;
        break;
      }
    }
    if (valid) {
      location << _state;
    }
  }
  /* Add the country code (if present) */
  if (!_country.isEmpty()) {
    location << _country;
  }
  return location.join(", ");
}

