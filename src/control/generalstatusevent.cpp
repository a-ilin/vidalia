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
 * \file generalstatusevent.cpp
 * \version $Id$
 * \brief Base class for all general Tor status events
 */

#include "generalstatusevent.h"


/** Returns a GeneralStatusEvent::Status enum value for the status value
 * represented by <b>str</b>. */
GeneralStatusEvent::Status
GeneralStatusEvent::statusFromString(const QString &str)
{
  if (!str.compare("CLOCK_JUMPED", Qt::CaseInsensitive))
    return ClockJumped;
  if (!str.compare("DANGEROUS_VERSION", Qt::CaseInsensitive))
    return DangerousTorVersion;
  if (!str.compare("TOO_MANY_CONNECTIONS", Qt::CaseInsensitive))
    return TooManyConnections;
  if (!str.compare("BUG", Qt::CaseInsensitive))
    return TorBug;
  if (!str.compare("CLOCK_SKEW", Qt::CaseInsensitive))
    return ClockSkew;
  if (!str.compare("BAD_LIBEVENT", Qt::CaseInsensitive))
    return BadLibevent;
  if (!str.compare("DIR_ALL_UNREACHABLE", Qt::CaseInsensitive))
    return DirAllUnreachable;
  return UnrecognizedStatus;
}

