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
 * \file geoipresponse.cpp
 * \version $Id$
 */

#include <QStringList>
#include "geoipresponse.h"

#define HTTP_OK   200     /**< HTTP OK status code. */


/** Constructor. Parses the response data for an HTTP header and Geo IP
 * information. */
GeoIpResponse::GeoIpResponse(QByteArray response)
{
  QString data(response);

  /* Parse out the header */
  int headerPos = data.indexOf("\r\n");
  _header = QHttpResponseHeader(data.mid(0, headerPos));
 
  /* Parse out the Geo IP information, if any was included. */
  if (headerPos > 0 && _header.statusCode() == HTTP_OK) {
    /* Split the content from the header */
    QString content = data.mid(headerPos+2);
    
    /* Split each response line at the \n */
    QStringList lines = content.split("\n");
    foreach (QString line, lines) {
      /* Parse the Geo IP information */
      GeoIp geoip = GeoIp::fromString(line);
      if (!geoip.isEmpty()) {
        /* Add it to the list of response information */
        _geoips << geoip;
      }
    }
  }
}

