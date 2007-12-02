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
 * \file clientstatusevent.cpp
 * \version $Id$
 * \brief Base class for all Tor client status events
 */

#include "clientstatusevent.h"


/** Returns a ClientStatusEvent::Status enum value for the status event type
 * represented by <b>str</b>. */
ClientStatusEvent::Status
ClientStatusEvent::statusFromString(const QString &str)
{
  if (!str.compare("ENOUGH_DIR_INFO", Qt::CaseInsensitive))
    return EnoughDirectoryInfo;
  if (!str.compare("NOT_ENOUGH_DIR_INFO", Qt::CaseInsensitive))
    return NotEnoughDirectoryInfo;
  if (!str.compare("CIRCUIT_ESTABLISHED", Qt::CaseInsensitive))
    return CircuitEstablished;
  if (!str.compare("CIRCUIT_NOT_ESTABLISHED", Qt::CaseInsensitive))
    return CircuitNotEstablished;
  if (!str.compare("DANGEROUS_SOCKS", Qt::CaseInsensitive))
    return DangerousSocks;
  if (!str.compare("SOCKS_UNKNOWN_PROTOCOL", Qt::CaseInsensitive))
    return UnknownSocksProtocol;
  if (!str.compare("SOCKS_BAD_HOSTNAME", Qt::CaseInsensitive))
    return SocksBadHostname;
  return UnrecognizedStatus;
}

