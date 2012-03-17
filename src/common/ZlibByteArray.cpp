/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
**
**                     *       *       *
**
**  Zlib support in this class is derived from Tor's torgzip.[ch].
**  Tor is distributed under this license:
**
**    Copyright (c) 2001-2004, Roger Dingledine
**    Copyright (c) 2004-2006, Roger Dingledine, Nick Mathewson
**
**   Redistribution and use in source and binary forms, with or without
**   modification, are permitted provided that the following conditions are
**   met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above
**       copyright notice, this list of conditions and the following disclaimer
**       in the documentation and/or other materials provided with the
**       distribution.
**
**     * Neither the names of the copyright owners nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
**    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
**    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
**    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
**    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
**    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
**    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
** \file ZlibByteArray.cpp
** \brief Wrapper around QByteArray that adds compression capabilities
*/

#include "config.h"

#include <QString>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#elif defined(HAVE_SYS_LIMITS_H)
#include <sys/limits.h>
#endif

/* The following check for UINT_MAX is derived from Tor's torint.h. See
 * the top of this file for details on Tor's license. */
#ifndef UINT_MAX
#if (SIZEOF_INT == 2)
#define UINT_MAX 0xffffu
#elif (SIZEOF_INT == 4)
#define UINT_MAX 0xffffffffu
#elif (SIZEOF_INT == 8)
#define UINT_MAX 0xffffffffffffffffu
#else
#error "Your platform uses a sizeof(int) that we don't understand."
#endif
#endif

#include "zlib.h"
#include <ZlibByteArray.h>


/** Constructor */
ZlibByteArray::ZlibByteArray(QByteArray data)
: QByteArray(data)
{
}

/** Return the 'bits' value to tell zlib to use <b>method</b>.*/
int
ZlibByteArray::methodBits(CompressionMethod method)
{
  /* Bits+16 means "use gzip" in zlib >= 1.2 */
  return (method == Gzip ? 15+16 : 15);
}

/** Returns a string description of <b>method</b>. */
QString
ZlibByteArray::methodString(CompressionMethod method)
{
  switch (method) {
    case None:  return "None";
    case Zlib:  return "Zlib";
    case Gzip:  return "Gzip";
    default:    return "Unknown";
  }
}

/** Returns true if the Zlib compression library is available and usable. */
bool
ZlibByteArray::isZlibAvailable()
{
  static int isZlibAvailable = -1;
  if (isZlibAvailable >= 0)
    return isZlibAvailable;

  /* From zlib.h:
   * "The application can compare zlibVersion and ZLIB_VERSION for consistency.
   * If the first character differs, the library code actually used is
   * not compatible with the zlib.h header file used by the application." */
  QString libVersion(zlibVersion());
  QString headerVersion(ZLIB_VERSION);
  if (libVersion.isEmpty() || headerVersion.isEmpty() ||
      libVersion.at(0) != headerVersion.at(0))
    isZlibAvailable = 0;
  else
    isZlibAvailable = 1;

  return isZlibAvailable;
}

/** Returns true iff we support gzip-based compression. Otherwise, we need to
 * use zlib. */
bool
ZlibByteArray::isGzipSupported()
{
  static int isGzipSupported = -1;
  if (isGzipSupported >= 0)
    return isGzipSupported;

  QString version(zlibVersion());
  if (version.startsWith("0.") ||
      version.startsWith("1.0") ||
      version.startsWith("1.1"))
    isGzipSupported = 0;
  else
    isGzipSupported = 1;

  return isGzipSupported;
}

/** Compresses the current contents of this object using <b>method</b>.
 * Returns the  compressed data if successful. If an error occurs, this will
 * return an empty QByteArray and set the optional <b>errmsg</b> to a string
 * describing the failure. */
QByteArray
ZlibByteArray::compress(const CompressionMethod method,
                        QString *errmsg) const
{
  return compress(QByteArray(data()), method, errmsg);
}

/** Compresses <b>in</b> using <b>method</b>. Returns the compressed data
 * if successful. If an error occurs, this will return an empty QByteArray and
 * set the optional <b>errmsg</b> to a string describing the failure. */
QByteArray
ZlibByteArray::compress(const QByteArray in,
                        const CompressionMethod method,
                        QString *errmsg)
{
  QByteArray out;
  QString errorstr;
  struct z_stream_s *stream = NULL;
  size_t out_size;
  size_t out_len;
  size_t in_len = in.length();
  off_t offset;

  if (method == None)
    return in;
  if (method == Gzip && !isGzipSupported()) {
    /* Old zlib versions don't support gzip in deflateInit2 */
    if (errmsg)
      *errmsg = QString("Gzip not supported with zlib %1")
                                        .arg(ZLIB_VERSION);
    return QByteArray();
  }

  stream = new struct z_stream_s;
  stream->zalloc = Z_NULL;
  stream->zfree = Z_NULL;
  stream->opaque = NULL;
  stream->next_in = (unsigned char*)in.data();
  stream->avail_in = in_len;

  if (deflateInit2(stream, Z_BEST_COMPRESSION, Z_DEFLATED,
                   methodBits(method),
                   8, Z_DEFAULT_STRATEGY) != Z_OK) {
    errorstr = QString("Error from deflateInit2: %1")
                 .arg(stream->msg ? stream->msg : "<no message>");
    goto err;
  }

  /* Guess 50% compression. */
  out_size = in_len / 2;
  if (out_size < 1024) out_size = 1024;

  out.resize(out_size);
  stream->next_out = (unsigned char*)out.data();
  stream->avail_out = out_size;

  while (1) {
    switch (deflate(stream, Z_FINISH))
    {
      case Z_STREAM_END:
        goto done;
      case Z_OK:
        /* In case zlib doesn't work as I think .... */
        if (stream->avail_out >= stream->avail_in+16)
          break;
      case Z_BUF_ERROR:
        offset = stream->next_out - ((unsigned char*)out.data());
        out_size *= 2;
        out.resize(out_size);
        stream->next_out = (unsigned char*)(out.data() + offset);
        if (out_size - offset > UINT_MAX) {
          errorstr =
            "Ran over unsigned int limit of zlib while uncompressing";
          goto err;
        }
          stream->avail_out = (unsigned int)(out_size - offset);
        break;
      default:
        errorstr = QString("%1 compression didn't finish: %2")
                     .arg(methodString(method))
                     .arg(stream->msg ? stream->msg : "<no message>");
        goto err;
    }
  }
done:
  out_len = stream->total_out;
  if (deflateEnd(stream)!=Z_OK) {
    errorstr = "Error freeing zlib structures";
    goto err;
  }
  out.resize(out_len);
  delete stream;
  return out;
err:
  if (stream) {
    deflateEnd(stream);
    delete stream;
  }
  if (errmsg)
    *errmsg = errorstr;
  return QByteArray();
}

/** Uncompresses the current contents of this object using <b>method</b>.
 * Returns the uncompressed data if successful. If an error occurs, this will
 * return an empty QByteArray and set the optional <b>errmsg</b> to a string
 * describing the failure. */
QByteArray
ZlibByteArray::uncompress(const CompressionMethod method,
                          QString *errmsg) const
{
  return uncompress(QByteArray(data()), method, errmsg);
}

/** Uncompresses <b>in</b> using <b>method</b>. Returns the uncompressed data
 * if successful. If an error occurs, this will return an empty QByteArray and
 * set the optional <b>errmsg</b> to a string describing the failure. */
QByteArray
ZlibByteArray::uncompress(const QByteArray in,
                          const CompressionMethod method,
                          QString *errmsg)
{
  QByteArray out;
  QString errorstr;
  struct z_stream_s *stream = NULL;
  size_t out_size;
  size_t out_len;
  size_t in_len = in.length();
  off_t offset;
  int r;

  if (method == None)
    return in;
  if (method == Gzip && !isGzipSupported()) {
    /* Old zlib versions don't support gzip in inflateInit2 */
    if (errmsg)
      *errmsg = QString("Gzip not supported with zlib %1")
                                        .arg(ZLIB_VERSION);
    return QByteArray();
  }

  stream = new struct z_stream_s;
  stream->zalloc = Z_NULL;
  stream->zfree = Z_NULL;
  stream->opaque = NULL;
  stream->msg = NULL;
  stream->next_in = (unsigned char*) in.data();
  stream->avail_in = in_len;

  if (inflateInit2(stream,
                   methodBits(method)) != Z_OK) {
    errorstr = QString("Error from inflateInit2: %1")
                 .arg(stream->msg ? stream->msg : "<no message>");
    goto err;
  }

  out_size = in_len * 2;  /* guess 50% compression. */
  if (out_size < 1024) out_size = 1024;

  out.resize(out_size);
  stream->next_out = (unsigned char*)out.data();
  stream->avail_out = out_size;

  while (1) {
    switch (inflate(stream, Z_FINISH))
    {
      case Z_STREAM_END:
        if (stream->avail_in == 0)
          goto done;
        /* There may be more compressed data here. */
        if ((r = inflateEnd(stream)) != Z_OK) {
          errorstr = "Error freeing zlib structures";
          goto err;
        }
        if (inflateInit2(stream, methodBits(method)) != Z_OK) {
          errorstr = QString("Error from second inflateInit2: %1")
                       .arg(stream->msg ? stream->msg : "<no message>");
          goto err;
        }
        break;
      case Z_OK:
        if (stream->avail_in == 0)
          goto done;
        /* In case zlib doesn't work as I think.... */
        if (stream->avail_out >= stream->avail_in+16)
          break;
        case Z_BUF_ERROR:
          if (stream->avail_out > 0) {
            errorstr = QString("Possible truncated or corrupt %1 data")
                         .arg(methodString(method));
            goto err;
          }
          offset = stream->next_out - (unsigned char*)out.data();
          out_size *= 2;
          out.resize(out_size);
          stream->next_out = (unsigned char*)(out.data() + offset);
          if (out_size - offset > UINT_MAX) {
            errorstr =
              "Ran over unsigned int limit of zlib while uncompressing";
            goto err;
          }
            stream->avail_out = (unsigned int)(out_size - offset);
          break;
        default:
          errorstr = QString("%1 decompression returned an error: %2")
                       .arg(methodString(method))
                       .arg(stream->msg ? stream->msg : "<no message>");
          goto err;
    }
  }
done:
  out_len = stream->next_out - (unsigned char*)out.data();
  r = inflateEnd(stream);
  delete stream;
  if (r != Z_OK) {
    errorstr = "Error freeing zlib structure";
    goto err;
  }
  out.resize(out_len);
  return out;
err:
  if (stream) {
    inflateEnd(stream);
    delete stream;
  }
  if (errmsg)
    *errmsg = errorstr;
  return QByteArray();
}

