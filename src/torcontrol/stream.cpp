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
** \file stream.cpp
** \version $Id$
** \brief Object representing a Tor stream
*/

#include <QStringList>
 
#include "stream.h"


/** Default constructor. */
Stream::Stream()
{
  _streamId  = 0;
  _status    = Unknown;
  _circuitId = 0;
  _port      = 0;
}

/** Constructor */
Stream::Stream(quint64 streamId, Status status, quint64 circuitId, 
               QString address, quint16 port)
{
  _streamId  = streamId;
  _status    = status;
  _circuitId = circuitId;
  _address   = address;
  _port      = port;
}

/** Constructor */
Stream::Stream(quint64 streamId, Status status, quint64 circuitId, QString target)
{
  _streamId  = streamId;
  _status    = status;
  _circuitId = circuitId;
  _port      = 0;

  int i = target.indexOf(":");
  if (i >= 0)
    _address = target.mid(0, i);
  if (i + 1 < target.length()) 
    _port = target.mid(i+1).toUInt();
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
  } else if (strStatus == "REMAP") {
    status = Remap;
  } else {
    status = Unknown;
  }
  return status;
}

/** Returns a human-understandable string representation of this 
 * stream's status. */
QString
Stream::statusString() const
{
  QString status;
  switch (_status) {
    case New:           status = tr("New"); break;
    case NewResolve:    
    case SentResolve:   status = tr("Resolving"); break;
    case SentConnect:   status = tr("Connecting"); break;
    case Succeeded:     status = tr("Open"); break;
    case Failed:        status = tr("Failed"); break;
    case Closed:        status = tr("Closed"); break;
    case Detached:      status = tr("Retrying"); break;
    case Remap:         status = tr("Remapped"); break;
    default:            status = tr("Unknown"); break;
  }
  return status;
}

/** Returns true if all fields in this Stream object are empty. */
bool
Stream::isEmpty() const
{
  return (!_streamId && !_circuitId && 
          (_status == Unknown) && _address.isEmpty() && !_port);
}

