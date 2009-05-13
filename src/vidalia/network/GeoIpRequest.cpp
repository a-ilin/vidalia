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
** \file GeoIpRequest.cpp
** \version $Id$
** \brief A formatted request for GeoIP information for one or more IPs
*/

#include "GeoIpRequest.h"
#include "ZlibByteArray.h"

#include <QString>
#include <QHostAddress>
#include <QHttpRequestHeader>


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
  _request = "format=long&ip=";
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

