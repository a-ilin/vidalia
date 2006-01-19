/****************************************************************
*  $Id$
* 
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

#include "torevents.h"

/* Include the event types */
#include "bandwidthevent.h"
#include "circuitevent.h"
#include "streamevent.h"
#include "logevent.h"

/** Default constructor */
TorEvents::TorEvents()
{
}

/** Default destructor */
TorEvents::~TorEvents()
{
}

/** Converts an event type to a string Tor understands */
QString
TorEvents::toString(TorEvent e)
{
  QString event;
  switch (e) {
    case Bandwidth: event = "BW"; break;
    case LogDebug: event = "DEBUG"; break;
    case LogInfo: event = "INFO"; break;
    case LogNotice: event = "NOTICE"; break;
    case LogWarn: event = "WARN"; break;
    case LogError: event = "ERR"; break;
    case Circuit: event = "CIRC"; break;
    case Stream: event = "STREAM"; break;
    default: event = "UNKNOWN"; break;
  }
  return event;
}

TorEvents::TorEvent
TorEvents::toTorEvent(QString event)
{
  TorEvent e;
  event = event.toUpper();
  if (event == "BW") {
    e = Bandwidth;
  } else if (event == "CIRCUIT") {
    e = Circuit;
  } else if (event == "STREAM") {
    e = Stream;
  } else if (event == "DEBUG") {
    e = LogDebug;
  } else if (event == "NOTICE") {
    e = LogNotice;
  } else if (event == "INFO") {
    e = LogInfo;
  } else if (event == "NOTICE") {
    e = LogNotice;
  } else if (event == "WARN") {
    e = LogWarn;
  } else if (event == "ERR") {
    e = LogError;
  } else {
    e = Unknown;
  }
  return e;
}

/** Parse the event type out of a message line and return the corresponding
 * Event enum value */
TorEvents::TorEvent
TorEvents::parseEventType(ReplyLine line)
{
  QString msg = line.getMessage();
  int i = msg.indexOf(" ");
  return toTorEvent(msg.mid(0, i));
}

/** Handles an event message from Tor. An event message can potentially have
 * more than one line, so we will iterate through them all and dispatch the
 * necessary events. */
void
TorEvents::handleEvent(ControlReply reply)
{
  foreach(ReplyLine line, reply.getLines()) {
    switch (parseEventType(line)) {
      case Bandwidth:  handleBandwidthUpdate(line); break;
      case Circuit:    handleCircuitStatus(line); break;
      case Stream:     handleStreamStatus(line); break;
    
      case LogDebug: 
      case LogInfo:
      case LogNotice:
      case LogWarn:
      case LogError:
        handleLogMessage(line); break;
      default: break;
    }
  }
}

/** Handle a bandwidth update event, to inform the controller of the bandwidth
 * used in the last second. The format of this message is:
 *
 *     "650" SP "BW" SP BytesRead SP BytesWritten
 *     BytesRead = 1*DIGIT
 *     BytesWritten = 1*DIGIT
 */
void
TorEvents::handleBandwidthUpdate(ReplyLine line)
{
  QStringList msg = line.getMessage().split(" ");
  if (msg.size() >= 3) {
    quint64 bytesIn = (quint64)msg.at(1).toULongLong();
    quint64 bytesOut = (quint64)msg.at(2).toULongLong();
    
    /*
      Post the new event here
    */
  }
}

/** Handle a circuit status event. The format of this message is:
 *
 *    "650" SP "CIRC" SP CircuitID SP CircStatus SP Path
 *    CircStatus =
 *             "LAUNCHED" / ; circuit ID assigned to new circuit
 *             "BUILT"    / ; all hops finished, can now accept streams
 *             "EXTENDED" / ; one more hop has been completed
 *             "FAILED"   / ; circuit closed (was not built)
 *             "CLOSED"     ; circuit closed (was built)
 *    Path = ServerID *("," ServerID)
 */
void
TorEvents::handleCircuitStatus(ReplyLine line)
{
  QStringList msg = line.getMessage().split(" ");
  if (msg.size() >= 4) {
    quint64 circId = (quint64)msg.at(1).toULongLong();
    CircuitEvent::Status status = CircuitEvent::toStatus(msg.at(2));
    QString path = msg.at(3);
  
    /*
      Post the new event here
    */
  }
}

/** Handle a stream status event. The format of this message is:
 *     
 *    "650" SP "STREAM" SP StreamID SP StreamStatus SP CircID SP Target SP 
 *     StreamStatus =
 *                 "NEW"          / ; New request to connect
 *                 "NEWRESOLVE"   / ; New request to resolve an address
 *                 "SENTCONNECT"  / ; Sent a connect cell along a circuit
 *                 "SENTRESOLVE"  / ; Sent a resolve cell along a circuit
 *                 "SUCCEEDED"    / ; Received a reply; stream established
 *                 "FAILED"       / ; Stream failed and not retriable.
 *                 "CLOSED"       / ; Stream closed
 *                 "DETACHED"       ; Detached from circuit; still retriable.
 *      Target = Address ":" Port
 *
 *  If the circuit ID is 0, then the stream is unattached.      
 */
void
TorEvents::handleStreamStatus(ReplyLine line)
{
  QStringList msg = line.getMessage().split(" ");
  if (msg.size() >= 4) {
    quint64 streamId = (quint64)msg.at(1).toULongLong();
    StreamEvent::Status status = StreamEvent::toStatus(msg.at(2));
    quint64 circId = (quint64)msg.at(3).toULongLong();
    QString target = msg.at(4);

    /*
      Post the new event here
    */
  }
}

/** Handle a log message event. The format of this message is:
 *  The syntax is:
 *
 *     "650" SP Severity SP ReplyText
 *       or
 *     "650+" Severity CRLF Data
 *     Severity = "DEBUG" / "INFO" / "NOTICE" / "WARN"/ "ERR"
 */
void
TorEvents::handleLogMessage(ReplyLine line)
{
  QString msg = line.getMessage();
  int i = msg.indexOf(" ");
  LogEvent::Severity severity = LogEvent::toSeverity(msg.mid(0, i));
  
  /* Determine if the message spanned multiple lines or not. */
  /* 
    Post the new event here
  if (line.getData().size() > 0) {
    emit log(severity, line.getData().join("\n"));
  } else {
    emit log(severity, msg.mid(i+1));
  }
  */
}

