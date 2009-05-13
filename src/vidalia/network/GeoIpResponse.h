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
** \file GeoIpResponse.h
** \version $Id$
** \brief Parses a response to a previous GeoIP request
*/

#ifndef _GEOIPRESPONSE_H
#define _GEOIPRESPONSE_H

#include <QList>
#include <QByteArray>
#include <QHttpResponseHeader>

class GeoIp;
class QString;
class QStringList;


class GeoIpResponse
{
public:
  /** Constructor. Parses the response data for an HTTP header and Geo IP
   * information.  */
  GeoIpResponse(const QByteArray &response);

  /** Returns the HTTP status code for this response.
   */
  int statusCode() const;

  /** Returns the HTTP status message for this response.
   */
  QString statusMessage() const;

  /** Returns the Geo IP information contained in this response.
   */
  QByteArray content() const;
  
private:
  /** Decodes a <b>chunked</b> transfer encoding. Returns the unchunked 
   * result on success, or an empty QByteArray if decoding fails. */
  QByteArray decodeChunked(const QByteArray &chunked);
  
  QHttpResponseHeader _header; /**< HTTP response header. */
  QByteArray _content; /**< Geo IP information in this response. */
};

#endif

