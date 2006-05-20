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
 * \file stream.cpp
 * \version $Id$
 */

#include <QStringList>
 
#include "stream.h"


/** Default constructor. */
Stream::Stream()
{
  _streamId  = 0;
  _status    = Unknown;
  _circuitId = 0;
  _target    = QString();
}

/** Constructor */
Stream::Stream(quint64 streamId, Status status, quint64 circuitId, QString target)
{
  _streamId  = streamId;
  _status    = status;
  _circuitId = circuitId;
  _target    = target;
}

/** Parses the given string for stream information, given in Tor control
 * protocol format. The format is:
 *
 *     StreamID SP StreamStatus SP CircID SP Target
 */
Stream
Stream::fromString(QString stream)
{
  QStringList parts = stream.split(" ");
  if (parts.size() >= 4) { 
    /* Get the stream ID */
    quint64 streamId = (quint64)parts.at(0).toULongLong();
    /* Get the stream status value */
    Stream::Status status = Stream::toStatus(parts.at(1));
    /* Get the ID of the circuit on which this stream travels */
    quint64 circId = (quint64)parts.at(2).toULongLong();
    /* Get the target address for this stream */
    QString target = parts.at(3);
    
    return Stream(streamId, status, circId, target);
  }
  return Stream();
}

/** Converts a string description of a stream's status to its enum value */
Stream::Status
Stream::toStatus(QString strStatus)
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

/** Returns true if all fields in this Stream object are empty. */
bool
Stream::isEmpty()
{
  return (!_streamId && !_circuitId && 
          (_status == Unknown) && _target.isEmpty());
}

