/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
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
 ***************************************************************/

/**
 * \file geoiprequest.cpp
 * \version $Id$
 * \brief A formatted request for GeoIP information for one or more IPs
 */

#include <zlibbytearray.h>

#include "geoiprequest.h"


/** Creates an HTTP POST header for this request, based on the 
 * Host, Page, and content-length values. */
QHttpRequestHeader
GeoIpRequest::createHeader() const
{
  QHttpRequestHeader header("POST", _page, 1, 1);
  
  if (!_host.isEmpty())
    header.setValue("Host", _host);
  header.setContentType("application/x-www-form-urlencoded");
  header.setContentLength(_request.length());
  header.setValue("Connection", "close");

  if (ZlibByteArray::isZlibAvailable()) {
    QString acceptEncodings = "deflate, x-deflate";
    if (ZlibByteArray::isGzipSupported())
      acceptEncodings += ", gzip, x-gzip";
    header.setValue("Accept-Encoding", acceptEncodings);
  }

  return header;
}

/** Sets the list of IPs whose geo information we want to request. */
void
GeoIpRequest::setRequest(const QList<QHostAddress> &ips)
{
  _request = "ip=";
  int ipcount = ips.size();

  /* Add each IP to a comma-delimited list. */
  for (int i = 0; i < ipcount; i++) {
    _request.append(ips.at(i).toString());
    if (i < ipcount-1) {
      _request.append(",");
    }
  }
  _ips = ips;
}

/** Formats the request as an HTTP POST request. */
QByteArray
GeoIpRequest::request() const
{
  /* Create the header and append the request content. */
  QString request = createHeader().toString() + _request;
  return request.toAscii();
}

/** Returns true if this request contains <b>ip</b>. */
bool
GeoIpRequest::contains(const QHostAddress &ip) const
{
  return _ips.contains(ip);
}

