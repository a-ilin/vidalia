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
 * \file stream.h
 * \version $Id$
 */

#ifndef _STREAM_H
#define _STREAM_H

#include <QString>


class Stream
{
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
    Detached      /**< Detached from circuit */
  };
    
  /** Default constructor */
  Stream();
  /** Constructor */
  Stream(quint64 streamId, Status status, quint64 circuitId, QString target);
  
  /** Converts a string description of a stream's status to its enum value */
  static Status toStatus(QString strStatus);
  
  /** Returns the ID for this stream */
  quint64 id() { return _streamId; }
  /** Returns the status for this stream */
  Status status() { return _status; }
  /** Returns the ID of the circuit to which this stream is assigned */
  quint64 circuitId() { return _circuitId; }
  /** Returns the target for this stream */
  QString target() { return _target; }
  
private:
  quint64 _streamId;   /**< Unique ID associated with this stream. */
  Status  _status;     /**< Stream status value. */
  quint64 _circuitId;  /**< ID of the circuit carrying this stream. */
  QString _target;     /**< Stream target address. */
};

#endif

