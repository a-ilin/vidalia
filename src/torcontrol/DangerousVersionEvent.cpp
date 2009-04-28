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
** \file DangerousVersionEvent.cpp
** \version $Id$
** \brief Event sent when Tor realizes its version is not recommended
*/

#include "DangerousVersionEvent.h"


/** Returns a DangerousVersionEvent::Reason enum value for <b>str</b>,
 * representing the reason why Tor thinks its version is dangerous. */
DangerousVersionEvent::Reason
DangerousVersionEvent::reasonFromString(const QString &str)
{
  if (!str.compare("UNRECOMMENDED", Qt::CaseInsensitive))
    return UnrecommendedVersion;
  if (!str.compare("OLD", Qt::CaseInsensitive)
        || !str.compare("OBSOLETE", Qt::CaseInsensitive))
    return ObsoleteVersion;
  if (!str.compare("NEW", Qt::CaseInsensitive))
    return NewVersion;
  return UnrecognizedReason;
}

