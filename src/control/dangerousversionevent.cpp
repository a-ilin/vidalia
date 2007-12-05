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
 * \file dangerousversionevent.cpp
 * \version $Id$
 * \brief Event sent when Tor realizes its version is not recommended
 */

#include "dangerousversionevent.h"


/** Returns a DangerousVersionEvent::Reason enum value for <b>str</b>,
 * representing the reason why Tor thinks its version is dangerous. */
DangerousVersionEvent::Reason
DangerousVersionEvent::reasonFromString(const QString &str)
{
  if (!str.compare("UNRECOMMENDED", Qt::CaseInsensitive))
    return UnrecommendedVersion;
  if (!str.compare("OLD", Qt::CaseInsensitive))
    return NewVersion;
  if (!str.compare("NEW", Qt::CaseInsensitive))
    return OldVersion;
  return UnrecognizedReason;
}

