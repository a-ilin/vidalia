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
 ****************************************************************/

/**
 * \file geoipresponse.cpp
 * \version $Id$
 * \brief Parses a response to a previous GeoIP request
 */

#include <QStringList>
#include <util/zlibbytearray.h>

#include "geoipresponse.h"

/** Status code for a successful HTTP request. */
#define STATUS_HTTP_OK                 200
/** Status code for content encoding errors. */
#define STATUS_CONTENT_ENCODING_ERR    601
/** Status code for transfer encoding errors. */
#define STATUS_TRANSFER_ENCODING_ERR   602


/** Constructor. Parses the response data for an HTTP header and Geo IP
 * information. */
GeoIpResponse::GeoIpResponse(QByteArray response)
{
  QString errmsg;
  
  /* Parse out the header */
  int headerPos = response.indexOf("\r\n\r\n");
  _header = QHttpResponseHeader(QString(response.mid(0, headerPos)));

  /* Parse out the Geo IP information, if any was included. */
  if (headerPos > 0 && _header.statusCode() == STATUS_HTTP_OK) {
    QByteArray content = response.mid(headerPos+4);
 
    if (_header.hasKey("Transfer-Encoding")) {
      QString encoding = _header.value("Transfer-Encoding");
      if (encoding == "chunked") {
        content = decodeChunked(content);
        if (content.isEmpty()) {
          _header.setStatusLine(STATUS_TRANSFER_ENCODING_ERR,
            QString("Failed to decode chunked response"));
          return;
        }
      } else {
        _header.setStatusLine(STATUS_TRANSFER_ENCODING_ERR,
          QString("Unknown transfer encoding '%1'").arg(encoding));
        return;
      }
    }
 
    if (_header.hasKey("Content-Encoding")) {
      ZlibByteArray::CompressionMethod method;
      QString encoding = _header.value("Content-Encoding");
      if (encoding == "gzip" || encoding == "x-gzip") {
        method = ZlibByteArray::Gzip;
      } else if (encoding == "deflate" || encoding == "x-deflate") {
        method = ZlibByteArray::Zlib;
      } else if (encoding == "text/plain") {
        method = ZlibByteArray::None;
      } else {
        _header.setStatusLine(STATUS_CONTENT_ENCODING_ERR,
          QString("Unknown content encoding '%1'").arg(encoding));
        return;
      }
 
      content = ZlibByteArray::uncompress(content, method, &errmsg);
      if (content.isEmpty()) {
        _header.setStatusLine(STATUS_CONTENT_ENCODING_ERR,
          QString("Content decoding using method '%1' failed: %2")
                                       .arg(encoding).arg(errmsg));
        return;
      }
    }

    /* Parse the Geo IP information in each line */
    QStringList lines = QString(content).split("\n");
    foreach (QString line, lines) {
      GeoIp geoip = GeoIp::fromString(line);
      if (!geoip.isEmpty())
        _geoips << geoip;
    }
  }
}

/** Decodes a <b>chunked</b> transfer encoding. Returns the unchunked 
 * result on success, or an empty QByteArray if decoding fails. */
QByteArray
GeoIpResponse::decodeChunked(QByteArray chunked)
{
  QByteArray unchunked;
  QString sizeString;
  int eol, chunkedlen, chunksize, offset = 0;
  bool ok;
 
  chunkedlen = chunked.length();
  while (offset < chunkedlen) {
    eol = chunked.indexOf("\r\n", offset);
    if (eol < 0)
      return QByteArray();
    sizeString = QString::fromAscii(chunked.mid(offset, eol-offset));
    offset = eol + 2; /* Skip past the CRLF */
    
    if (sizeString.indexOf(";") >= 0)
      sizeString.truncate(sizeString.indexOf(";")); 
    chunksize = sizeString.toInt(&ok, 16);
    if (!ok || chunksize > chunkedlen - offset)
      return QByteArray();
    if (!chunksize)
      break; /* Last chunk. Ignore the trailer. */
    
    unchunked.append(chunked.mid(offset, chunksize));
    offset += chunksize;
    offset += 2; /* CRLF after each chunk */
  }
  return unchunked;
}

