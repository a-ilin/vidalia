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
** \file geoip.h
** \version $Id$
** \brief Associates an IP with a geographic location
*/

#ifndef _GEOIP_H
#define _GEOIP_H

#include <QString>
#include <QHostAddress>


class GeoIp
{
public:
  /** Default constructor */
  GeoIp() : _latitude(0xFFFF), _longitude(0xFFFF) {}
  /** Constructor. */
  GeoIp(QHostAddress ip);

  /** Constructor */
  GeoIp(QHostAddress ip, float latitude, float longitude, 
        QString city, QString state, QString country);
  
  /** Creates a GeoIp object from a string. */
  static GeoIp fromString(QString geoip);
  /** Builds a comma-delimited string of GeoIp fields. */
  QString toString() const;

  /** Returns the IP address for this object. */
  QHostAddress ip() const { return _ip; }
  /** Returns the latitude coordinate for this IP. */
  float latitude() const { return _latitude; }
  /** Returns the longitude coordinate for this IP. */
  float longitude() const { return _longitude; }
  /** Returns the city in which this IP lives. */
  QString city() const { return _city; }
  /** Returns the state or district in which this IP lives. */
  QString state() const { return _state; }
  /** Returns the country in which this IP lives. */
  QString country() const { return _country; }
  /** Returns a human-readable string of city, region(state), and country. */
  QString toLocation() const;

  /** Returns true if the GeoIp object is invalid. */
  bool isEmpty() const;
  /** Returns true if the GeoIp object is valid, but no location information
   * is known for the associated IP address. */
   bool isUnknown() const;

private:
  QHostAddress _ip; /**< IP address for this location. */
  float _latitude;  /**< Latitudinal coordinate for this IP's location. */
  float _longitude; /**< Longitudinal coordinate for this IP's location. */
  QString _city;    /**< City in which this IP lives. */
  QString _state;   /**< State or district in which this IP lives. */
  QString _country; /**< Country in which this IP lives. */
};

#endif

