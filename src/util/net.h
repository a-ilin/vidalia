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
 * \file net.h
 * \version $Id$
 */

#ifndef _NET_H
#define _NET_H

#include <QHostAddress>

/** Returns the local machine's IP address. */
QHostAddress net_local_address();

/** Returns true if the given address is a private IP. */
bool net_is_private_address(QHostAddress addr);

/** Returns true if the given IP address is a valid IPv4 address. */
bool net_is_valid_ip(QString ip);

/** Tries to determine this machine's external IP. */
bool net_get_public_ip(QString &ip);

#endif

