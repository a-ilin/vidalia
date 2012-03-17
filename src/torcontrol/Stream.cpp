/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file Stream.cpp
** \brief Object representing a Tor stream
*/

#include "Stream.h"
#include "Circuit.h"

#include <QStringList>


/** Default constructor. */
Stream::Stream()
{
  _status    = Unknown;
  _port      = 0;
}

/** Constructor */
Stream::Stream(const StreamId &streamId, Status status,
               const CircuitId &circuitId, const QString &address,
               quint16 port)
{
  _streamId  = streamId;
  _status    = status;
  _circuitId = circuitId;
  _address   = address;
  _port      = port;
}

/** Constructor */
Stream::Stream(const StreamId &streamId, Status status,
               const CircuitId &circuitId, const QString &target)
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
Stream::fromString(const QString &stream)
{
  QStringList parts = stream.split(" ", QString::SkipEmptyParts);
  if (parts.size() >= 4) {
    /* Get the stream ID */
    StreamId streamId = parts.at(0);
    /* Get the stream status value */
    Stream::Status status = Stream::toStatus(parts.at(1));
    /* Get the ID of the circuit on which this stream travels */
    CircuitId circId = parts.at(2);
    /* Get the target address for this stream */
    QString target = parts.at(3);

    return Stream(streamId, status, circId, target);
  }
  return Stream();
}

/** Returns true iff <b>streamId</b> consists of only between 1 and 16
 * (inclusive) ASCII-encoded letters and numbers. */
bool
Stream::isValidStreamId(const StreamId &streamId)
{
  int length = streamId.length();
  if (length < 1 || length > 16)
    return false;

  for (int i = 0; i < length; i++) {
    char c = streamId[i].toAscii();
    if (c < '0' && c > '9' && c < 'A' && c > 'Z' && c < 'a' && c > 'z')
      return false;
  }
  return true;
}

/** Converts a string description of a stream's status to its enum value */
Stream::Status
Stream::toStatus(const QString &strStatus)
{
  if (!strStatus.compare("NEW", Qt::CaseInsensitive))
    return New;
  if (!strStatus.compare("NEWRESOLVE", Qt::CaseInsensitive))
    return NewResolve;
  if (!strStatus.compare("SENTCONNECT", Qt::CaseInsensitive))
    return SentConnect;
  if (!strStatus.compare("SENTRESOLVE", Qt::CaseInsensitive))
    return SentResolve;
  if (!strStatus.compare("SUCCEEDED", Qt::CaseInsensitive))
    return Succeeded;
  if (!strStatus.compare("FAILED", Qt::CaseInsensitive))
    return Failed;
  if (!strStatus.compare("CLOSED", Qt::CaseInsensitive))
    return Closed;
  if (!strStatus.compare("DETACHED", Qt::CaseInsensitive))
    return Detached;
  if (!strStatus.compare("REMAP", Qt::CaseInsensitive))
    return Remap;
  return Unknown;
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

/** Returns true if all fields in this Stream object are valid. */
bool
Stream::isValid() const
{
  return (isValidStreamId(_streamId)
            && Circuit::isValidCircuitId(_circuitId)
            && (_status != Unknown)
            && !_address.isEmpty());
}

