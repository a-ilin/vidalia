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
 * \file statusevent.cpp
 * \version $Id$
 * \brief Base class for all Tor status events
 */

#include "statusevent.h"


/** Returns a StatusEvent::Severity enum value for the severity represented
 * by <b>str</b>. */ 
StatusEvent::Severity
StatusEvent::severityFromString(const QString &str)
{
  if (!str.compare("NOTICE", Qt::CaseInsensitive))
    return SeverityNotice;
  else if (!str.compare("WARN", Qt::CaseInsensitive))
    return SeverityWarn;
  else if (!str.compare("ERR", Qt::CaseInsensitive))
    return SeverityError;
  return SeverityUnknown;
}

