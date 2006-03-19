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

/** 
 * \file streamevent.cpp
 * \version $Id$
 */

#include "eventtype.h"
#include "streamevent.h"

/** Default constructor */
StreamEvent::StreamEvent(quint64 streamId, Status status, 
                         quint64 circuitId, QString target)
: QEvent((QEvent::Type)CustomEventType::StreamEvent)
{
  _streamId  = streamId;
  _status    = status;
  _circuitId = circuitId;
  _target    = target;
}

/** Converts a string description of a stream's status to its enum value */
StreamEvent::Status
StreamEvent::toStatus(QString strStatus)
{
  Status status;
  strStatus = strStatus.toUpper();
  if (strStatus == "NEW") {
    status = New;
  } else if (strStatus == "NEWRESOLVE") {
    status = NewResolve;
  } else if (strStatus == "SENTCONNECT") {
    status = SentConnect;
  } else if (strStatus == "SENTRESOLVE") {
    status = SentResolve;
  } else if (strStatus == "SUCCEEDED") {
    status = Succeeded;
  } else if (strStatus == "FAILED") {
    status = Failed;
  } else if (strStatus == "CLOSED") {
    status = Closed;
  } else if (strStatus == "DETACHED") {
    status = Detached;
  } else {
    status = Unknown;
  }
  return status;
}

/** Returns the ID assigned to this stream */
quint64
StreamEvent::streamId()
{
  return _streamId;
}

/** Returns the status of this stream */
StreamEvent::Status
StreamEvent::status()
{
  return _status;
}

/** Returns the ID of the circuit to which this stream is assigned */
quint64
StreamEvent::circuitId()
{
  return _circuitId;
}

/** Returns the target for this stream */
QString
StreamEvent::target()
{
  return _target;
}
