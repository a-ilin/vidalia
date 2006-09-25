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
 * \file geoiprequest.h
 * \version $Id$
 * \brief A formatted request for GeoIP information for one or more IPs
 */

#ifndef _GEOIPREQUEST_H
#define _GEOIPREQUEST_H

#include <QList>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QHttpRequestHeader>


class GeoIpRequest
{
public:
  /** Constructor */
  GeoIpRequest(int id) : _id(id) {}
  
  /** Sets the Host: field in this request's header. */
  void setHost(QString host) { _host = host; }
  /** Sets the page path in this request's header. */
  void setPage(QString page) { _page = page; }
  /** Sets the list of IPs whose geo information we want to request. */
  void setRequest(QList<QHostAddress> ips);
  
  /** Returns the request's identifier. */
  int id() { return _id; }
  /** Formats the request as an HTTP POST request */
  QByteArray request();
  
private:
  /** Creates an HTTP header for this request. */
  QHttpRequestHeader createHeader();
  
  int _id;          /**< Request identifier */
  QString _host;    /**< Host: field value. */
  QString _page;    /**< Page giving us the geo ip information. */
  QString _request; /**< Formatted Geo IP request string. */
};

#endif

