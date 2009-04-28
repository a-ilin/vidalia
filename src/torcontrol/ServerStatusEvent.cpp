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
** \file ServerStatusEvent.cpp
** \version $Id$
** \brief Base class for all Tor server status events
*/

#include "ServerStatusEvent.h"


/** Returns a ServerStatusEvent::Status enum value for the server status
 * represented by <b>str</b>. */
ServerStatusEvent::Status
ServerStatusEvent::statusFromString(const QString &str)
{
  if (!str.compare("EXTERNAL_ADDRESS", Qt::CaseInsensitive))
    return ExternalAddressChanged;
  if (!str.compare("CHECKING_REACHABILITY", Qt::CaseInsensitive))
    return CheckingReachability;
  if (!str.compare("REACHABILITY_SUCCEEDED", Qt::CaseInsensitive))
    return ReachabilitySucceeded;
  if (!str.compare("GOOD_SERVER_DESCRIPTOR", Qt::CaseInsensitive))
    return GoodServerDescriptor;
  if (!str.compare("NAMESERVER_STATUS", Qt::CaseInsensitive))
    return NameserverStatusChanged;
  if (!str.compare("NAMESERVER_ALL_DOWN", Qt::CaseInsensitive))
    return NameserversAllDown;
  if (!str.compare("DNS_HIJACKED", Qt::CaseInsensitive))
    return DnsHijacked;
  if (!str.compare("DNS_USELESS", Qt::CaseInsensitive))
    return DnsUseless;
  if (!str.compare("BAD_SERVER_DESCRIPTOR", Qt::CaseInsensitive))
    return RejectedServerDescriptor;
  if (!str.compare("ACCEPTED_SERVER_DESCRIPTOR", Qt::CaseInsensitive))
    return AcceptedServerDescriptor;
  if (!str.compare("REACHABILITY_FAILED", Qt::CaseInsensitive))
    return ReachabilityFailed;
  return UnrecognizedStatus;
}

