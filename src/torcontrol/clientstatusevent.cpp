/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/* 
** \file clientstatusevent.cpp
** \version $Id$
** \brief Base class for all Tor client status events
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
  if (!str.compare("DANGEROUS_PORT", Qt::CaseInsensitive))
    return DangerousPort;
  if (!str.compare("DANGEROUS_SOCKS", Qt::CaseInsensitive))
    return DangerousSocks;
  if (!str.compare("SOCKS_UNKNOWN_PROTOCOL", Qt::CaseInsensitive))
    return UnknownSocksProtocol;
  if (!str.compare("SOCKS_BAD_HOSTNAME", Qt::CaseInsensitive))
    return SocksBadHostname;
  if (!str.compare("BOOTSTRAP", Qt::CaseInsensitive))
    return Bootstrap;
  return UnrecognizedStatus;
}

