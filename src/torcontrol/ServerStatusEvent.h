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
** \file ServerStatusEvent.h
** \version $Id$
** \brief Base class for all Tor server status events
*/

#ifndef _SERVERSTATUSEVENT_H
#define _SERVERSTATUSEVENT_H

#include "StatusEvent.h"


class ServerStatusEvent : public StatusEvent
{
public:
  /**< Tor server status event types. */
  enum Status {
    UnrecognizedStatus,
    ExternalAddressChanged,
    NameserverStatusChanged,
    NameserversAllDown,
    DnsHijacked,
    DnsUseless,
    AcceptedServerDescriptor,
    RejectedServerDescriptor,
    GoodServerDescriptor,
    CheckingReachability,
    ReachabilityFailed,
    ReachabilitySucceeded
  };

  /** Constructor */
  ServerStatusEvent(tc::Severity severity, Status status)
    : StatusEvent((QEvent::Type)CustomEventType::ServerStatusEvent, severity),
      _status(status) {}

  /** Returns the server status indicated by this event. */
  Status status() const { return _status; }
  /** Returns a ServerStatusEvent::Status enum value for the server status
   * represented by <b>str</b>. */
  static Status statusFromString(const QString &str);

private:
  Status _status; /**< Tor Server status indicated by this event. */
};

#endif

