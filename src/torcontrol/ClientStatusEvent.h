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
** \file ClientStatusEvent.h
** \version $Id$
** \brief Base class for all Tor client status events
*/

#ifndef _CLIENTSTATUSEVENT_H
#define _CLIENTSTATUSEVENT_H

#include "StatusEvent.h"


class ClientStatusEvent : public StatusEvent
{
public:
  /** Tor client status event type. */
  enum Status {
    UnrecognizedStatus,
    Bootstrap,
    CircuitEstablished,
    CircuitNotEstablished,
    NotEnoughDirectoryInfo,
    EnoughDirectoryInfo,
    DangerousPort,
    DangerousSocks,
    UnknownSocksProtocol,
    SocksBadHostname
  };
  
  /** Constructor. */
  ClientStatusEvent(tc::Severity severity, Status status)
    : StatusEvent((QEvent::Type)CustomEventType::ClientStatusEvent, severity),
       _status(status) {}

  /** Returns the client status indicated by this event. */
  Status status() const { return _status; }
  /** Returns a ClientStatusEvent::Status enum value for the status event type
   * represented by <b>str</b>. */
  static Status statusFromString(const QString &str);

private:
  Status _status; /**< Tor Client status indicated by this event. */
};

#endif

