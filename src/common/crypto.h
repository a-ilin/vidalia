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
**  Pseudorandom number generation support in this file is derived from
**  Tor's crypto.[ch]. Tor is distributed under this license.
**
**    Copyright (c) 2001-2004, Roger Dingledine
**    Copyright (c) 2004-2007, Roger Dingledine, Nick Mathewson
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
** \file crypto.h
** \brief Provides support for pseuodrandom number generation.
*/

#ifndef _CRYPTO_H
#define _CRYPTO_H

#include <QByteArray>
#include <QString>

/** Returns <b>len</b> bytes of pseudorandom data on success, or an empty
 * QByteArray on failure. This function is based on crypto_seed_rng() from
 * Tor's crypto.c. See LICENSE for details on Tor's license. */
QByteArray crypto_rand_bytes(int len);
/** Returns a pseudorandom integer, chosen uniformly from the the values in
 * the range [0, max). This function is based on crypto_rand_int() from Tor's
 * crypto.c. See LICENSE for details on Tor's license. */
quint32 crypto_rand_quint32(quint32 max);
/** Generates a pseudorandom string of length <b>len</b> containing printable
 * ASCII characters from the range '!' (0x21) to '~' (0x7e). */
QString crypto_rand_string(int len);
/** Generates a salted hash of <b>secret</b> using the random <b>salt</b>
 * according to the iterated and salted S2K algorithm in RFC 2440. <b>c</b>
 * is the one-octet coded count value that specifies how much data to hash.
 * <b>salt</b> must contain at least 8 bytes, otherwise this method will
 * return a default-constructed QByteArray. */
QByteArray
crypto_secret_to_key(const QString &secret, const QByteArray &salt, quint8 c);

#endif

