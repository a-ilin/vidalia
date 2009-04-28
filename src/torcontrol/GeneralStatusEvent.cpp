/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to
**  the terms described in the LICENSE file.
*/

/*
** \file GeneralStatusEvent.cpp
** \version $Id$
** \brief Base class for all general Tor status events
*/

#include "GeneralStatusEvent.h"


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

