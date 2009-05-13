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
** \file GeoIpRequest.h
** \version $Id$
** \brief A formatted request for GeoIP information for one or more IPs
*/

#ifndef _GEOIPREQUEST_H
#define _GEOIPREQUEST_H

#include <QList>
#include <QString>
#include <QHttpRequestHeader>

class QHostAddress;
class QByteArray;


class GeoIpRequest
{
public:
  /** Constructor */
  GeoIpRequest(int id) : _id(id) {}
  
  /** Sets the Host: field in this request's header. */
  void setHost(const QString &host) { _host = host; }
  /** Sets the page path in this request's header. */
  void setPage(const QString &page) { _page = page; }
  /** Sets the list of IPs whose geo information we want to request. */
  void setRequest(const QList<QHostAddress> &ips);
  /** Returns true if this request contains <b>ip</b>. */
  bool contains(const QHostAddress &ip) const;

  /** Returns the request's identifier. */
  int id() const { return _id; }
  /** Returns the number of IP addresses contained in this request. */
  int size() const { return _ips.size(); }
  /** Formats the request as an HTTP POST request */
  QByteArray request() const;
  
private:
  /** Creates an HTTP header for this request. */
  QHttpRequestHeader createHeader() const;
  
  int _id;          /**< Request identifier */
  QString _host;    /**< Host: field value. */
  QString _page;    /**< Page giving us the geo ip information. */
  QString _request; /**< Formatted Geo IP request string. */
  QList<QHostAddress> _ips; /**< List of IP addresses in this request. */
};

#endif

