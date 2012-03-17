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
** \file ZlibByteArray.h
** \brief Wrapper around QByteArray that adds compression capabilities
*/

#ifndef _ZLIBBYTEARRAY_H
#define _ZLIBBYTEARRAY_H

#include <QByteArray>
#include <QString>


class ZlibByteArray : public QByteArray
{
public:
  /** Available compression methods. */
  enum CompressionMethod {
    None,   /**< No compression method. */
    Gzip,   /**< Gzip compression method. */
    Zlib    /**< Zlib compression method. */
  };

  /** Constructor. */
  ZlibByteArray(QByteArray data);

  /** Compresses the current contents of this object using <b>method</b>. */
  QByteArray compress(const CompressionMethod method = Zlib,
                      QString *errmsg = 0) const;
  /** Compreses the contents of <b>in</b> using <b>method</b>. */
  static QByteArray compress(const QByteArray in,
                             const CompressionMethod method = Zlib,
                             QString *errmsg = 0);
  /** Uncompresses the current contents of this object using <b>method</b>. */
  QByteArray uncompress(CompressionMethod method = Zlib,
                        QString *errmsg = 0) const;
  /** Uncompresses the contents of <b>in</b> using <b>method</b>. */
  static QByteArray uncompress(const QByteArray in,
                               const CompressionMethod method = Zlib,
                               QString *errmsg = 0);

  /** Returns true if the Zlib compression library is available and usable. */
  static bool isZlibAvailable();
  /** Returns true iff we support gzip-based compression. Otherwise, we need to
    * use zlib. */
  static bool isGzipSupported();

private:
  /** Return the 'bits' value to tell zlib to use <b>method</b>.*/
  static int methodBits(CompressionMethod method);
  /** Returns a string description of <b>method</b>. */
  static QString methodString(CompressionMethod method);
};

#endif

