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
 */

#ifndef _GEOIP_H
#define _GEOIP_H

#include <QString>
#include <QHostAddress>


class GeoIp
{
public:
  /** Default constructor */
  GeoIp() : _latitude(0), _longitude(0) {}

  /** Constructor */
  GeoIp(QHostAddress ip, float latitude, float longitude, 
        QString city, QString state, QString country)
  : _ip(ip), _latitude(latitude), _longitude(longitude),
    _city(city), _state(state), _country(country) {} 

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
  
private:
  QHostAddress _ip; /**< IP address for this location. */
  float _latitude;  /**< Latitudinal coordinate for this IP's location. */
  float _longitude; /**< Longitudinal coordinate for this IP's location. */
  QString _city;    /**< City in which this IP lives. */
  QString _state;   /**< State or district in which this IP lives. */
  QString _country; /**< Country in which this IP lives. */
};

#endif

