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
 * \file clientstatusevent.h
 * \version $Id$
 * \brief Base class for all Tor client status events
 */

#ifndef _CLIENTSTATUSEVENT_H
#define _CLIENTSTATUSEVENT_H

#include "statusevent.h"


class ClientStatusEvent : public StatusEvent
{
public:
  /** Tor client status event type. */
  enum Status {
    UnrecognizedStatus,
    CircuitEstablished,
    CircuitNotEstablished,
    NotEnoughDirectoryInfo,
    EnoughDirectoryInfo,
    DangerousSocks,
    UnknownSocksProtocol,
    SocksBadHostname
  };
  
  /** Constructor. */
  ClientStatusEvent(StatusEvent::Severity severity, Status status)
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

