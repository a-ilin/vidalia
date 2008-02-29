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
** \file stream.h
** \version $Id$
** \brief Object representing a Tor stream
*/

#ifndef _STREAM_H
#define _STREAM_H

#include <QCoreApplication>
#include <QString>
#include <QObject>

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
  Stream(quint64 streamId, Status status, quint64 circuitId, QString target);
  /** Constructor */
  Stream(quint64 streamId, Status status, quint64 circuitId,
         QString address, quint16 port);

  /** Parses the given string for a stream, in Tor control protocol format. */
  static Stream fromString(QString stream);
  /** Converts a string description of a stream's status to its enum value */
  static Status toStatus(QString strStatus);

  /** Returns true if the Stream object's fields are all empty. */
  bool isEmpty() const;
  
  /** Returns the ID for this stream. */
  quint64 id() const { return _streamId; }
  /** Returns the status for this stream. */
  Status status() const { return _status; }
  /** Returns a string representation of this stream's status. */
  QString statusString() const;
  /** Returns the ID of the circuit to which this stream is assigned. */
  quint64 circuitId() const { return _circuitId; }
  /** Returns the target address and port for this stream. */
  QString target() const { return (_address + ":" + QString::number(_port)); }
  /** Returns the target address for this stream. */
  QString targetAddress() const { return _address; }
  /** Returns the target port for this stream. */
  quint16 targetPort() const { return _port; }

private:
  quint64 _streamId;   /**< Unique ID associated with this stream. */
  Status  _status;     /**< Stream status value. */
  quint64 _circuitId;  /**< ID of the circuit carrying this stream. */
  QString _address;    /**< Stream target address. */
  quint16 _port;       /**< Stream target port. */
};

#endif

