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
 * \file geoipresponse.h
 * \version $Id$
 */

#ifndef _GEOIPRESPONSE_H
#define _GEOIPRESPONSE_H

#include <QList>
#include <QByteArray>
#include <QHttpResponseHeader>

#include "geoip.h"


class GeoIpResponse
{
public:
  /** Constructor. Parses the response data for an HTTP header and Geo IP
   * information.  */
  GeoIpResponse(QByteArray response);

  /** Returns the HTTP status code for this response. */
  int statusCode() { return _header.statusCode(); }
  /** Returns the HTTP status message for this response. */
  QString statusMessage() { return _header.reasonPhrase(); }
  /** Returns the Geo IP information contained in this response. */
  QList<GeoIp> geoIps() { return _geoips; }
  
private:
  QHttpResponseHeader _header; /**< HTTP response header. */
  QList<GeoIp> _geoips;        /**< Geo IP information in this response. */
};

#endif

