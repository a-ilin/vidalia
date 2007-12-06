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
 * \file serverstatusevent.cpp
 * \version $Id$
 * \brief Base class for all Tor server status events
 */

#include "serverstatusevent.h"


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

