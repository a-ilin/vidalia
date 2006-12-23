/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/**
 * \file geoip.h
 * \version $Id$
 * \brief Associates an IP with a geographic location
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

