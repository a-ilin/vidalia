/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
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

#include "orconnevent.h"

#include "eventtype.h"

/** Constructor
 * \param status OR connection status.
 * \param server OR server.
 */
OrConnEvent::OrConnEvent(Status status, QString server)
: QEvent((QEvent::Type)CustomEventType::OrConnEvent)
{
  _status   = status;
  _server = server;
}

/** Converts a string description of a connection's status to an enum value */
OrConnEvent::Status
OrConnEvent::toStatus(QString status)
{
  Status s;
  status = status.toUpper();
  if (status == "NEW") {
    s = New;
  } else if (status == "LAUNCHED") {
    s = Launched;
  } else if (status == "CONNECTED") {
    s = Connected;
  } else if (status == "FAILED") {
    s = Failed;
  } else if (status == "CLOSED") {
    s = Closed;
  } else {
    s = Unknown;
  }
  return s;
}

/** Returns the status for this OR connection. */
OrConnEvent::Status
OrConnEvent::status()
{
  return _status;
}

/** Returns the OR server with which this connection is associated. */
QString
OrConnEvent::server()
{
  return _server;
}

