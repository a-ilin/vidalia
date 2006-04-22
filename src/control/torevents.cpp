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
 * \file torevents.cpp
 * \version $Id$
 */

#include <QApplication>

#include "torevents.h"

/** Default constructor */
TorEvents::TorEvents()
{
}

/** Adds an event and interested object to the list */
void
TorEvents::add(TorEvent e, QObject *obj)
{
  if (!_eventList.values(e).contains(obj)) {
    _eventList.insert(e, obj);
  }
}

/** Removes an event and object from the event list */
void
TorEvents::remove(TorEvent e, QObject *obj)
{
  int i;
  if ((i = _eventList.values(e).indexOf(obj)) >= 0) {
    _eventList.values(e).removeAt(i);
    if (_eventList.values(e).size() == 0) {
      _eventList.remove(e);
    }
  }
}

/** Returns true if an event has any registered handlers */
bool
TorEvents::contains(TorEvent event)
{
  if (_eventList.contains(event)) {
    return (_eventList.values(event).count() > 0);
  }
  return false;
}

/** Returns the list of events in which we're interested */
QList<TorEvents::TorEvent>
TorEvents::eventList()
{
  return _eventList.keys();
}

/** Dispatches a given event to all its handler targets. */
void
TorEvents::dispatch(TorEvent e, QEvent *event)
{
  foreach (QObject *obj, _eventList.values(e)) {
    QApplication::postEvent(obj, event);
  }
}

/** Converts an event type to a string Tor understands */
QString
TorEvents::toString(TorEvent e)
{
  QString event;
  switch (e) {
    case Bandwidth: event = "BW"; break;
    case LogDebug:  event = "DEBUG"; break;
    case LogInfo:   event = "INFO"; break;
    case LogNotice: event = "NOTICE"; break;
    case LogWarn:   event = "WARN"; break;
    case LogError:  event = "ERR"; break;
    case Circuit:   event = "CIRC"; break;
    case Stream:    event = "STREAM"; break;
    case OrConn:    event = "ORCONN"; break;
    case NewDescriptor: event = "NEWDESC"; break;
    default: event = "UNKNOWN"; break;
  }
  return event;
}

/** Converts a log severity to its related Tor event */
TorEvents::TorEvent
TorEvents::toTorEvent(LogEvent::Severity severity)
{
  TorEvent e;
  switch (severity) {
    case LogEvent::TorDebug:  e = LogDebug; break;
    case LogEvent::TorInfo:   e = LogInfo; break;
    case LogEvent::TorNotice: e = LogNotice; break;
    case LogEvent::TorWarn:   e = LogWarn; break;
    case LogEvent::TorError:  e = LogError; break;
    default: e = Unknown; break;
  }
  return e;
}

/** Converts an event in the string form sent by Tor to its enum value */
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
  } else if (event == "ORCONN") {
    e = OrConn;
  } else if (event == "NEWDESC") {
    e = NewDescriptor;
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
      case OrConn:     handleOrConnStatus(line); break;
      case NewDescriptor: handleNewDescriptor(line); break;

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
  
    /* Post the event to each of the interested targets */
    dispatch(Bandwidth, new BandwidthEvent(bytesIn, bytesOut));
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
 
    /* Post the event to each of the interested targets */
    dispatch(Circuit, new CircuitEvent(circId, status, path));
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
    QString targetAddr = msg.at(4);

    /* Post the event to each of the interested targets */
    dispatch(Stream, new StreamEvent(streamId, status, circId, targetAddr));
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
  QString logLine = (line.getData().size() > 0 ? line.getData().join("\n") :
                                                 msg.mid(i+1));
  
  /* Post the event to each of the interested targets */
  dispatch(toTorEvent(severity), new LogEvent(severity, logLine));
}

/** Handle an OR Connection Status event. The syntax is:
 *     "650" SP "ORCONN" SP (ServerID / Target) SP ORStatus
 *
 *     ORStatus = "NEW" / "LAUNCHED" / "CONNECTED" / "FAILED" / "CLOSED"
 *
 *     NEW is for incoming connections, and LAUNCHED is for outgoing
 *     connections. CONNECTED means the TLS handshake has finished (in
 *     either direction). FAILED means a connection is being closed
 *     that hasn't finished its handshake, and CLOSED is for connections
 *     that have handshaked.
 *
 *     A ServerID is specified unless it's a NEW connection, in which
 *     case we don't know what server it is yet, so we use Address:Port.
 */
void
TorEvents::handleOrConnStatus(ReplyLine line)
{
  QStringList msg = line.getMessage().split(" ");
  if (msg.size() >= 3) {
    dispatch(OrConn, new OrConnEvent(OrConnEvent::toStatus(msg.at(2)), 
                                     msg.at(1)));
  }
}

/** Handles a new descriptor event. The format for event messages of this type
 * is:
 *  
 *   "650" SP "NEWDESC" 1*(SP ServerID)
 */
void
TorEvents::handleNewDescriptor(ReplyLine line)
{
  QString descs = line.getMessage();
  QStringList descList = descs.mid(descs.indexOf(" ")+1).split(" ");
  if (descList.size() > 0) {
    dispatch(NewDescriptor, new NewDescriptorEvent(descList));
  }
}

