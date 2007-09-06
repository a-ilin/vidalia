/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
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
 * 
 *                     *       *       *
 * 
 *  Pseudorandom number generation support in this file is derived from
 *  Tor's crypto.[ch]. Tor is distributed under this license.
 * 
 *    Copyright (c) 2001-2004, Roger Dingledine
 *    Copyright (c) 2004-2007, Roger Dingledine, Nick Mathewson
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are
 *   met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution.
 * 
 *     * Neither the names of the copyright owners nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************/

/**
 * \file crypto.h
 * \version $Id$
 * \brief Provides support for pseuodrandom number generation.
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

#endif

