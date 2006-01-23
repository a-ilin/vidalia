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

#include <QApplication>

#include "torevents.h"

/** Default constructor */
TorEvents::TorEvents()
{
}

/** Default destructor */
TorEvents::~TorEvents()
{
}

/** Adds an event and interested object to the list */
void
TorEvents::add(TorEvent e)
{
  if (!_eventList.contains(e)) {
    _eventList.append(e);
  }
}

/** Removes an event and object from the event list */
void
TorEvents::remove(TorEvent e)
{
  int i;
  if ((i = _eventList.indexOf(e)) >= 0) {
    _eventList.removeAt(i);
  }
}

/** Returns true if an event has any registered handlers */
bool
TorEvents::contains(TorEvent e)
{
  return _eventList.contains(e);
}

/** Converts a Signal enum value to a SIGNAL() */
const char*
TorEvents::toSignal(Signal s)
{
  const char *signal = 0;
  switch (s) {
    case BandwidthSignal:
      signal = SIGNAL(bandwidth(quint64, quint64)); 
      break;
    case LogSignal:
      signal = SIGNAL(log(LogEvent::Severity, QString)); 
      break;
    case CircuitSignal:
      signal = SIGNAL(circuit(quint64, CircuitEvent::Status, QString));
      break;
    case StreamSignal:
      signal = SIGNAL(stream(quint64, StreamEvent::Status, quint64, QString));
      break;
  }
  return signal;
}

/** Connects a slot to the specified Tor event signal */
bool
TorEvents::connect(Signal s, QObject *obj, const char *slot)
{
  return QObject::connect(this, toSignal(s), obj, slot);
}

/** Disconnects a slot from one of the Tor event signals */
bool
TorEvents::disconnect(Signal s, QObject *obj, const char *slot)
{
  return QObject::disconnect(this, toSignal(s), obj, slot);
}

/** Returns the list of events in which we're interested */
QList<TorEvents::TorEvent>
TorEvents::eventList()
{
  return _eventList;
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
    case LogEvent::Debug:  e = LogDebug; break;
    case LogEvent::Info:   e = LogInfo; break;
    case LogEvent::Notice: e = LogNotice; break;
    case LogEvent::Warn:   e = LogWarn; break;
    case LogEvent::Error:  e = LogError; break;
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
  
    /* Let connected slots know about this bandwidth update */
    emit bandwidth(bytesIn, bytesOut);
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
 
    /* Let connected slots know about this circuit status event */
    emit circuit(circId, status, path);
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

    /* Let connected slots know about this stream status event */
    emit stream(streamId, status, circId, targetAddr);
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
  
  /* Let connected slots know about this log event */
  emit log(severity, logLine);
}

