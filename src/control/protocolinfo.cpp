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
 ****************************************************************/

/** 
 * \file protocolinfo.cpp
 * \version $Id$
 * \brief Container for information in a PROTOCOLINFO reply from Tor
 */

#include "protocolinfo.h"


/** Returns true if this ProtocolInfo object contains no data. */
bool
ProtocolInfo::isEmpty() const
{
  return (_torVersion.isEmpty() && 
         _authMethods.isEmpty() &&
         _cookieAuthFile.isEmpty());
}

/** Sets the authentication methods Tor currently accepts. <b>methods</b>
 * should be a comma-delimited list of authentication methods. */
void
ProtocolInfo::setAuthMethods(const QString authMethods)
{
  _authMethods = authMethods.split(",");
}

