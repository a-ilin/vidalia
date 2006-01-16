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

/** Default constructor */
TorEvents::TorEvents()
{
}

/** Default destructor */
TorEvents::~TorEvents()
{
}

const char *
TorEvents::toSignal(Signal sig)
{
  const char *signal;
  switch (sig) {
    case BandwidthSignal: 
      signal = SIGNAL(bandwidth(quint64, quint64));
      break;
    case CircuitSignal:
      signal = SIGNAL(circuit(quint64, CircuitStatus, QString));
      break;
    case StreamSignal:
      signal = SIGNAL(stream(quint64, StreamStatus, quint64, QString));
      break;
    case LogSignal:
      signal = SIGNAL(log(LogSeverity, QString));
      break;
    default:
      return signal = 0;
      break;
  }
  return signal;
}

/** Converts an event type to a string Tor understands */
QString
TorEvents::toString(Event e)
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

/** Parse the event type out of a message line and return the corresponding
 * Event enum value */
TorEvents::Event
TorEvents::parseEventType(ReplyLine line)
{
  QString msg = line.getMessage();
  int i = msg.indexOf(" ");
  QString event = msg.mid(0, i).toUpper();

  if (event == "BW") {
    return Bandwidth;
  } else if (event == "CIRCUIT") {
    return Circuit;
  } else if (event == "STREAM") {
    return Stream;
  } else if (event == "DEBUG") {
    return LogDebug;
  } else if (event == "NOTICE") {
    return LogNotice;
  } else if (event == "INFO") {
    return LogInfo;
  } else if (event == "NOTICE") {
    return LogNotice;
  } else if (event == "WARN") {
    return LogWarn;
  } else if (event == "ERR") {
    return LogError;
  }
  return Unknown;
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
    CircuitStatus status;
    quint64 circId = (quint64)msg.at(1).toULongLong();
    QString strStatus = msg.at(2).toUpper();
    QString path = msg.at(3);
    
    if (strStatus == "LAUNCHED") {
      status = CircuitLaunched;
    } else if (strStatus == "BUILT") {
      status = CircuitBuilt;
    } else if (strStatus == "EXTENDED") {
      status = CircuitExtended;
    } else if (strStatus == "FAILED") {
      status = CircuitFailed;
    } else if (strStatus == "CLOSED") {
      status = CircuitClosed;
    } else {
      return; /* Unknown status, so bail */
    }
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
    StreamStatus status;
    quint64 streamId = (quint64)msg.at(1).toULongLong();
    QString strStatus = msg.at(2).toUpper();
    quint64 circId = (quint64)msg.at(3).toULongLong();
    QString target = msg.at(4);

    if (strStatus == "NEW") {
      status = StreamNew;
    } else if (strStatus == "NEWRESOLVE") {
      status = StreamNewResolve;
    } else if (strStatus == "SENTCONNECT") {
      status = StreamSentConnect;
    } else if (strStatus == "SENTRESOLVE") {
      status = StreamSentResolve;
    } else if (strStatus == "SUCCEEDED") {
      status = StreamSucceeded;
    } else if (strStatus == "FAILED") {
      status = StreamFailed;
    } else if (strStatus == "CLOSED") {
      status = StreamClosed;
    } else if (strStatus == "DETACHED") {
      status = StreamDetached;
    } else {
      return; /* Unknown status, so bail */
    }
    emit stream(streamId, status, circId, target);
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
  QString strSeverity = msg.mid(0, i).toUpper();

  LogSeverity severity;
  if (strSeverity == "DEBUG") {
    severity = Debug;
  } else if (strSeverity == "INFO") {
    severity = Info;
  } else if (strSeverity == "NOTICE") {
    severity = Notice;
  } else if (strSeverity == "WARN") {
    severity = Warn;
  } else if (strSeverity == "ERR") {
    severity = Error;
  } else {
    return; /* Unknown severity, so bail */
  }
  
  /* Determine if the message spanned multiple lines or not. */
  if (line.getData().size() > 0) {
    emit log(severity, line.getData().join("\n"));
  } else {
    emit log(severity, msg.mid(i+1));
  }
}

