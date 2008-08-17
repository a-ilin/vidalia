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
** \file streamevent.h
** \version $Id$
** \brief Event dispatched when a stream status update is received from Tor
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
  StreamId id() const { return _stream.id(); }
  /** Returns the status for this stream event. */
  Stream::Status status() const { return _stream.status(); }
  /** Returns the ID of the circuit to which this stream is assigned */
  CircuitId circuitId() const { return _stream.circuitId(); }
  /** Returns the target for this stream event. */
  QString target() const { return _stream.target(); }
  
private:
  Stream _stream; /**< Stream object for this stream event. */
};

#endif

