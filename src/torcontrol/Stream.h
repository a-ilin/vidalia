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
** \file Stream.h
** \brief Object representing a Tor stream
*/

#ifndef _STREAM_H
#define _STREAM_H

#include "Circuit.h"

#include <QCoreApplication>
#include <QString>
#include <QObject>
#include <QList>
#include <QMetaType>

/** Stream IDs contains 1-16 alphanumeric ASCII characters. */
typedef QString StreamId;


class Stream
{
  Q_DECLARE_TR_FUNCTIONS(Stream)

public:
  /** Stream status values */
  enum Status {
    Unknown,      /**< Unknown status type given */
    New,          /**< New request to connect */
    NewResolve,   /**< New request to resolve an address */
    SentConnect,  /**< Sent a connect cell */
    SentResolve,  /**< Sent a resolve cell */
    Succeeded,    /**< Stream established */
    Failed,       /**< Stream failed */
    Closed,       /**< Stream closed */
    Detached,     /**< Detached from circuit */
    Remap         /**< Address re-mapped to another */
  };

  /** Default constructor */
  Stream();
  /** Constructor */
  Stream(const StreamId &streamId, Status status, const CircuitId &circuitId,
         const QString &target);
  /** Constructor */
  Stream(const StreamId &streamId, Status status, const CircuitId &circuitId,
         const QString &address, quint16 port);

  /** Parses the given string for a stream, in Tor control protocol format. */
  static Stream fromString(const QString &stream);
  /** Converts a string description of a stream's status to its enum value */
  static Status toStatus(const QString &strStatus);

  /** Returns true iff the Stream object's fields are all valid. */
  bool isValid() const;

  /** Returns the ID for this stream. */
  StreamId id() const { return _streamId; }
  /** Returns the status for this stream. */
  Status status() const { return _status; }
  /** Returns a string representation of this stream's status. */
  QString statusString() const;
  /** Returns the ID of the circuit to which this stream is assigned. */
  CircuitId circuitId() const { return _circuitId; }
  /** Returns the target address and port for this stream. */
  QString target() const { return (_address + ":" + QString::number(_port)); }
  /** Returns the target address for this stream. */
  QString targetAddress() const { return _address; }
  /** Returns the target port for this stream. */
  quint16 targetPort() const { return _port; }

  /** Returns true iff <b>streamId</b> consists of only between 1 and 16
   * (inclusive) ASCII-encoded letters and numbers. */
   static bool isValidStreamId(const StreamId &streamId);

private:
  StreamId _streamId;   /**< Unique ID associated with this stream. */
  CircuitId _circuitId; /**< ID of the circuit carrying this stream. */
  QString _address;    /**< Stream target address. */
  Status  _status;     /**< Stream status value. */
  quint16 _port;       /**< Stream target port. */
};

Q_DECLARE_METATYPE(Stream);

/** A collection of Stream objects. */
typedef QList<Stream> StreamList;

#endif

