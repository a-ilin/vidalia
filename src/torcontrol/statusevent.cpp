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
** \file statusevent.cpp
** \version $Id$
** \brief Base class for all Tor status events
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

