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
** \file GeoIpRecord.h
** \brief Associates an IP with a geographic location
*/

#ifndef _GEOIPRECORD_H
#define _GEOIPRECORD_H

#include <QHash>
#include <QString>
#include <QHostAddress>


class GeoIpRecord
{
public:
  /** Default constructor. Creates an empty GeoIpRecord object.
   */
  GeoIpRecord();

  /**
   */
  GeoIpRecord(const QHostAddress &ip, float latitude, float longitude,
              const QString &country, const QString &countryCode);

  /**
   */
  GeoIpRecord(const QHostAddress &ip, float latitude, float longitude,
              const QString &city, const QString &region,
              const QString &country, const QString &countryCode);

  /** Returns the IP address associated with this GeoIP object.
   */
  QHostAddress ip() const { return _ip; }

  /** Returns the latitude portion of the geographic coordinates associated
   * with this IP address or range of IP addresses.
   */
  float latitude() const { return _latitude; }

  /** Returns the longitude portion of the geographic coordinates associated
   * with this IP address or range of IP addresses.
   */
  float longitude() const { return _longitude; }

  /** Returns the name of the city associated with this IP address, if known.
   * Otherwise, returns an empty QString.
   */
  QString city() const { return _city; }

  /** Returns the full region name (e.g., state) in which this IP address
   * resides, if known. Otherwise, returns an empty QString.
   */
  QString region() const { return _region; }

  /** Returns the full name of the country associated with this IP address
   * or range of IP addresses, if known. Otherwise, returns an empty QString.
   */
  QString country() const { return _country; }

  /** Returns the ISO 3166-1 alpha-2 two-letter country code of the country
   * associated with this IP address or range of IP addresses, if known.
   * Otherwise, returns an empty QString.
   */
  QString countryCode() const { return _countryCode; }

  /** Returns a human-readable string of city, region(state), and country.
   * Some fields may be absent if they are not known. If no fields are known,
   * this will return an empty QString.
   */
  QString toString() const;

  /** Returns true if the GeoIpRecord object is valid. A valid GeoIpRecord object must
   * have valid IP address, valid latitude and longitude coordinates and a
   * two-letter country code.
   */
  bool isValid() const;

private:
  QHostAddress _ip; /**< IP address for this location. */
  float _latitude;  /**< Latitudinal coordinate for this IP's location. */
  float _longitude; /**< Longitudinal coordinate for this IP's location. */
  QString _city;    /**< City in which this IP lives. */
  QString _region;   /**< State or district in which this IP lives. */
  QString _country; /**< Country in which this IP lives. */
  QString _countryCode; /**< ISO-3166-1 alpha-2 country code. */
};

#endif

