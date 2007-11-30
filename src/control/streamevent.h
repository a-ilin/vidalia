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
 * \file streamevent.h
 * \version $Id$
 * \brief Event dispatched when a stream status update is received from Tor
 */

#ifndef _STREAMEVENT_H
#define _STREAMEVENT_H

#include <QEvent>

#include "eventtype.h"
#include "stream.h"


class StreamEvent : public QEvent
{
public:
  /** Default constructor */
  StreamEvent(Stream stream)
  : QEvent((QEvent::Type)CustomEventType::StreamEvent)
  { _stream = stream; }
  
  /** Returns the Stream object for this stream event. */
  Stream stream() const { return _stream; }
  /** Returns the ID for this stream event. */
  quint64 id() const { return _stream.id(); }
  /** Returns the status for this stream event. */
  Stream::Status status() const { return _stream.status(); }
  /** Returns the ID of the circuit to which this stream is assigned */
  quint64 circuitId() const { return _stream.circuitId(); }
  /** Returns the target for this stream event. */
  QString target() const { return _stream.target(); }
  
private:
  Stream _stream; /**< Stream object for this stream event. */
};

#endif

