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
** \file generalstatusevent.h
** \version $Id$
** \brief Base class for all general Tor status events
*/

#ifndef _GENERALSTATUSEVENT_H
#define _GENERALSTATUSEVENT_H

#include "statusevent.h"


class GeneralStatusEvent : public StatusEvent
{
public:
  /**< General Tor status event types. */
  enum Status {
    UnrecognizedStatus,
    ClockJumped,
    DangerousTorVersion,
    TooManyConnections,
    TorBug,
    ClockSkew,
    BadLibevent,
    DirAllUnreachable
  };

  /** Constructor */
  GeneralStatusEvent(tc::Severity severity, Status status)
    : StatusEvent((QEvent::Type)CustomEventType::ServerStatusEvent, severity),
      _status(status) {}

  /** Returns the general Tor status indicated by this event. */
  Status status() const { return _status; }
  /** Returns a GeneralStatusEvent::Status enum value for the status value
   * represented by <b>str</b>. */
  static Status statusFromString(const QString &str);

private:
  Status _status; /**< General Tor status value indicated by this event. */
};

#endif

