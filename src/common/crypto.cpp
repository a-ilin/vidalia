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
** \file crypto.cpp
** \brief Provides support for pseuodrandom number generation.
*/

#include "crypto.h"

#include <QFile>
#include <QStringList>
#include <QCryptographicHash>
#include <QtDebug>

#if defined(Q_OS_WIN32)
#include <windows.h>
#include <wincrypt.h>
#endif

#if defined(Q_OS_LINUX)
#include <stdint.h>
#include <arpa/inet.h>
#endif


/** Returns up to <b>len</b> bytes of pseudorandom data on success, or an empty
 * QByteArray on failure. The caller should verify that the returned
 * QByteArray contains the requested number of bytes. This function is based on
 * crypto_seed_rng() from Tor's crypto.c. See LICENSE for details on Tor's
 * license. */
QByteArray
crypto_rand_bytes(int len)
{
  QByteArray buf(len, 0);
#if defined(Q_OS_WIN32)
  static int provider_set = 0;
  static HCRYPTPROV provider;
#else
  static QStringList filenames =
    QStringList() << "/dev/srandom" << "/dev/urandom" << "/dev/random";
#endif
  Q_ASSERT(len > 0);

#if defined(Q_OS_WIN32)
  if (!provider_set) {
    if (!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL,
                             CRYPT_VERIFYCONTEXT)) {
      if ((unsigned long)GetLastError() != (unsigned long)NTE_BAD_KEYSET) {
        qWarning("Can't get CryptoAPI provider.");
        return QByteArray();
      }
    }
    provider_set = 1;
  }
  if (!CryptGenRandom(provider, buf.size(), (BYTE *)buf.data())) {
    qWarning("Can't get entropy from CryptoAPI.");
    return QByteArray();
  }
  return buf;
#else
  foreach (QString fname, filenames) {
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
      continue;

    qint64 bytes_read;
    qint64 total = 0;
    while (total < buf.size()) {
      bytes_read = file.read(buf.data()+total, buf.size()-total);
      if (bytes_read < 0)
        return QByteArray();
      else if (bytes_read == 0) {
        buf.resize(total);
        return buf;
      }
      total += bytes_read;
    }
    return buf;
  }
  qWarning("Can't read from /dev/*random.");
  return QByteArray();
#endif
}

/** Returns a pseudorandom integer, chosen uniformly from the the values in
 * the range [0, max). This function is based on crypto_rand_int() from Tor's
 * crypto.c. See LICENSE for details on Tor's license. */
quint32
crypto_rand_quint32(quint32 max)
{
  QByteArray buf;
  quint32 val;
  quint32 cutoff;
  Q_ASSERT(max > 0);

  cutoff = 0xffffffffu - (0xffffffffu % max);
  forever {
    buf = crypto_rand_bytes(sizeof(quint32));
    Q_ASSERT(buf.size() == sizeof(quint32));

    val = *((quint32 *)buf.constData());
    if (val < cutoff)
      break;
  }
  return (val % max);
}

/** Generates a pseudorandom string of length <b>len</b> containing printable
 * ASCII characters from the range '!' (0x21) to '~' (0x7e). */
QString
crypto_rand_string(int len)
{
  QString str;
  Q_ASSERT(len >= 0);

  for (int i = 0; i < len; i++)
    str += QChar('!' + crypto_rand_quint32('~'-'!'+1));
  return str;
}

/** Generates a salted hash of <b>secret</b> using the random <b>salt</b>
 * according to the iterated and salted S2K algorithm in RFC 2440. <b>c</b>
 * is the one-octet coded count value that specifies how much data to hash.
 * <b>salt</b> must contain at least 8 bytes, otherwise this method will
 * return a default-constructed QByteArray. */
QByteArray
crypto_secret_to_key(const QString &secret, const QByteArray &salt, quint8 c)
{
  if (salt.size() < 8)
    return QByteArray();

#define EXPBIAS 6
  int count = ((quint32)16 + (c & 15)) << ((c >> 4) + EXPBIAS);
#undef EXPBIAS

  QCryptographicHash hash(QCryptographicHash::Sha1);
  QByteArray tmp = salt.left(8).append(secret.toLatin1());
  while (count) {
    if (count > tmp.length()) {
      hash.addData(tmp);
      count -= tmp.length();
    } else {
      hash.addData(tmp.left(count));
      count = 0;
    }
  }

  return hash.result();
}

/**
 * Read a 32-bit value beginning at <b>cp</b>.  Equivalent to
 * *(uint32_t*)(cp), but will not cause segfaults on platforms that forbid
 * unaligned memory access.
 */
uint32_t
get_uint32(const void *cp)
{
  uint32_t v;
  memcpy(&v,cp,4);
  return v;
}
/**
 * Set a 32-bit value beginning at <b>cp</b> to <b>v</b>. Equivalent to
 * *(uint32_t*)(cp) = v, but will not cause segfaults on platforms that forbid
 * unaligned memory access. */
void
set_uint32(void *cp, uint32_t v)
{
  memcpy(cp,&v,4);
}

#define STMT_BEGIN do {
#define STMT_END } while(0)
#define SHA256_CTX sha256_state
#define SHA256_Init sha256_init
#define SHA256_Update sha256_process
#define LTC_ARGCHK(x) Q_ASSERT(x)
#include "sha256.c"
#define SHA256_Final(a,b) sha256_done(b,a)

static unsigned char *
SHA256(const unsigned char *m, size_t len, unsigned char *d)
{
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, m, len);
  SHA256_Final(d, &ctx);
  return d;
}

/** Compute the HMAC-SHA-256 of the <b>msg_len</b> bytes in <b>msg</b>, using
 * the <b>key</b> of length <b>key_len</b>.  Store the DIGEST256_LEN-byte
 * result in <b>hmac_out</b>.
 */
void
crypto_hmac_sha256(char *hmac_out,
                   const char *key, size_t key_len,
                   const char *msg, size_t msg_len)
{
#define BLOCKSIZE 64
#define DIGESTSIZE 32
  uint8_t k[BLOCKSIZE];
  uint8_t pad[BLOCKSIZE];
  uint8_t d[DIGESTSIZE];
  int i;
  SHA256_CTX st;

  if (key_len <= BLOCKSIZE) {
    memset(k, 0, sizeof(k));
    memcpy(k, key, key_len); /* not time invariant in key_len */
  } else {
    SHA256((const uint8_t *)key, key_len, k);
    memset(k+DIGESTSIZE, 0, sizeof(k)-DIGESTSIZE);
  }
  for (i = 0; i < BLOCKSIZE; ++i)
    pad[i] = k[i] ^ 0x36;
  SHA256_Init(&st);
  SHA256_Update(&st, pad, BLOCKSIZE);
  SHA256_Update(&st, (uint8_t*)msg, msg_len);
  SHA256_Final(d, &st);

  for (i = 0; i < BLOCKSIZE; ++i)
    pad[i] = k[i] ^ 0x5c;
  SHA256_Init(&st);
  SHA256_Update(&st, pad, BLOCKSIZE);
  SHA256_Update(&st, d, DIGESTSIZE);
  SHA256_Final((uint8_t*)hmac_out, &st);

  /* Now clear everything. */
  memset(k, 0, sizeof(k));
  memset(pad, 0, sizeof(pad));
  memset(d, 0, sizeof(d));
  memset(&st, 0, sizeof(st));
#undef BLOCKSIZE
#undef DIGESTSIZE
}
