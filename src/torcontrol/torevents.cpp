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
** \file torevents.cpp
** \version $Id$
** \brief Parses and dispatches events from Tor
*/

#include <QApplication>
#include <stringutil.h>

#include "circuit.h"
#include "stream.h"
#include "torevents.h"
#include "unrecognizedserverstatusevent.h"
#include "unrecognizedclientstatusevent.h"
#include "unrecognizedgeneralstatusevent.h"
#include "circuitestablishedevent.h"
#include "dangerousversionevent.h"
#include "bootstrapstatusevent.h"

/** Format of expiry times in address map events. */
#define DATE_FMT "\"yyyy-MM-dd HH:mm:ss\""


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

/** Removes <b>obj</b> from the list of target objects for event <b>e</b>. */
void
TorEvents::remove(TorEvent e, QObject *obj)
{
  QMultiHash<TorEvent,QObject*>::iterator i = _eventList.find(e);
  while (i != _eventList.end() && i.key() == e) {
    if (i.value() == obj) {
      _eventList.erase(i);
      break;
    }
    i++;
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
    case CircuitStatus:   event = "CIRC"; break;
    case StreamStatus:    event = "STREAM"; break;
    case OrConnStatus:    event = "ORCONN"; break;
    case NewDescriptor:   event = "NEWDESC"; break;
    case AddressMap:      event = "ADDRMAP"; break;
    case GeneralStatus:   event = "STATUS_GENERAL"; break;
    case ClientStatus:    event = "STATUS_CLIENT"; break;
    case ServerStatus:    event = "STATUS_SERVER"; break;
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
TorEvents::toTorEvent(const QString &event)
{
  TorEvent e;
  if (event == "BW") {
    e = Bandwidth;
  } else if (event == "CIRC") {
    e = CircuitStatus;
  } else if (event == "STREAM") {
    e = StreamStatus;
  } else if (event == "DEBUG") {
    e = LogDebug;
  } else if (event == "INFO") {
    e = LogInfo;
  } else if (event == "NOTICE") {
    e = LogNotice;
  } else if (event == "WARN") {
    e = LogWarn;
  } else if (event == "ERR") {
    e = LogError;
  } else if (event == "NEWDESC") {
    e = NewDescriptor;
  } else if (event == "ADDRMAP") {
    e = AddressMap;
  } else if (event == "STATUS_GENERAL") {
    e = GeneralStatus;
  } else if (event == "STATUS_CLIENT") {
    e = ClientStatus;
  } else if (event == "STATUS_SERVER") {
    e = ServerStatus;
  } else if (event == "ORCONN") {
    e = OrConnStatus;
  } else {
    e = Unknown;
  }
  return e;
}

/** Parse the event type out of a message line and return the corresponding
 * Event enum value */
TorEvents::TorEvent
TorEvents::parseEventType(const ReplyLine &line)
{
  QString msg = line.getMessage();
  int i = msg.indexOf(" ");
  return toTorEvent(msg.mid(0, i));
}

/** Handles an event message from Tor. An event message can potentially have
 * more than one line, so we will iterate through them all and dispatch the
 * necessary events. */
void
TorEvents::handleEvent(const ControlReply &reply)
{
  foreach(ReplyLine line, reply.getLines()) {
    switch (parseEventType(line)) {
      case Bandwidth:      handleBandwidthUpdate(line); break;
      case CircuitStatus:  handleCircuitStatus(line); break;
      case StreamStatus:   handleStreamStatus(line); break;
      case OrConnStatus:   handleOrConnStatus(line); break;
      case NewDescriptor:  handleNewDescriptor(line); break;
      case AddressMap:     handleAddressMap(line); break;

      case GeneralStatus:
        handleStatusEvent(GeneralStatus, line); break;
      case ClientStatus:
        handleStatusEvent(ClientStatus, line); break;
      case ServerStatus:
        handleStatusEvent(ServerStatus, line); break;

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
TorEvents::handleBandwidthUpdate(const ReplyLine &line)
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
TorEvents::handleCircuitStatus(const ReplyLine &line)
{
  QString msg = line.getMessage().trimmed();
  int i = msg.indexOf(" ") + 1;
  if (i > 0) {
    /* Post the event to each of the interested targets */
    Circuit circ(msg.mid(i));
    if (circ.isValid())
      dispatch(CircuitStatus, new CircuitEvent(circ));
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
TorEvents::handleStreamStatus(const ReplyLine &line)
{
  QString msg = line.getMessage().trimmed();
  int i  = msg.indexOf(" ") + 1;
  if (i > 0) {
    /* Post the event to each of the interested targets */
    dispatch(StreamStatus, new StreamEvent(Stream::fromString(msg.mid(i))));
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
TorEvents::handleLogMessage(const ReplyLine &line)
{
  QString msg = line.getMessage();
  int i = msg.indexOf(" ");
  LogEvent::Severity severity = LogEvent::toSeverity(msg.mid(0, i));
  QString logLine = (line.getData().size() > 0 ? line.getData().join("\n") :
                                                 msg.mid(i+1));

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
TorEvents::handleOrConnStatus(const ReplyLine &line)
{
  QStringList msg = line.getMessage().split(" ");
  if (msg.size() >= 3) {
    dispatch(OrConnStatus, 
      new OrConnEvent(OrConnEvent::toStatus(msg.at(2)), msg.at(1)));
  }
}

/** Handles a new descriptor event. The format for event messages of this type
 * is:
 *  
 *   "650" SP "NEWDESC" 1*(SP ServerID)
 */
void
TorEvents::handleNewDescriptor(const ReplyLine &line)
{
  QString descs = line.getMessage();
  QStringList descList = descs.mid(descs.indexOf(" ")+1).split(" ");
  if (descList.size() > 0) {
    dispatch(NewDescriptor, new NewDescriptorEvent(descList));
  }
}

/** Handles a new or updated address mapping event. The format for event
 * messages of this type is:
 *
 *   "650" SP "ADDRMAP" SP Address SP Address SP Expiry
 *   Expiry = DQUOTE ISOTime DQUOTE / "NEVER"
 *
 *   Expiry is expressed as the local time (rather than GMT).
 */
void
TorEvents::handleAddressMap(const ReplyLine &line)
{
  QStringList msg = line.getMessage().split(" ");
  if (msg.size() >= 4) {
    QDateTime expires;
    if (msg.size() >= 5 && msg.at(3) != "NEVER")
      expires = QDateTime::fromString(msg.at(3) + " " + msg.at(4), DATE_FMT);
    dispatch(AddressMap, new AddressMapEvent(msg.at(1), msg.at(2), expires));
  }
}

/** Handles a Tor status event. The format for event messages of this type is:
 *
 *  "650" SP StatusType SP StatusSeverity SP StatusAction
 *                                           [SP StatusArguments] CRLF
 *
 *  StatusType = "STATUS_GENERAL" / "STATUS_CLIENT" / "STATUS_SERVER"
 *  StatusSeverity = "NOTICE" / "WARN" / "ERR"
 *  StatusAction = 1*ALPHA
 *  StatusArguments = StatusArgument *(SP StatusArgument)
 *  StatusArgument = StatusKeyword '=' StatusValue
 *  StatusKeyword = 1*(ALNUM / "_")
 *  StatusValue = 1*(ALNUM / '_')  / QuotedString
 */
void
TorEvents::handleStatusEvent(TorEvent type, const ReplyLine &line)
{
  QString status;
  tc::Severity severity;
  QHash<QString,QString> args;
  QString msg = line.getMessage();
  
  severity = tc::toSeverity(msg.section(' ', 1, 1));
  status   = msg.section(' ', 2, 2);
  args     = string_parse_keyvals(msg.section(' ', 3));
  switch (type) {
    case ClientStatus:
      dispatchClientStatusEvent(severity, status, args); break;
    case ServerStatus:
      dispatchServerStatusEvent(severity, status, args); break;
    default:
      dispatchGeneralStatusEvent(severity, status, args);
  }
}

/** Parses and posts a Tor client status event. */
void
TorEvents::dispatchClientStatusEvent(tc::Severity severity,
                                     const QString &action,
                                     const QHash<QString,QString> &args)
{
  ClientStatusEvent *event;
  ClientStatusEvent::Status status 
    = ClientStatusEvent::statusFromString(action);

  switch (status) {
    case ClientStatusEvent::CircuitEstablished:
      event = new CircuitEstablishedEvent(severity); break;
    
    case ClientStatusEvent::Bootstrap:
      event = new BootstrapStatusEvent(BootstrapStatus(severity,
                    BootstrapStatus::statusFromString(args.value("TAG")),
                    args.value("PROGRESS").toInt(),
                    args.value("SUMMARY"),
                    args.value("WARNING"),
                    tc::toConnectionStatusReason(args.value("REASON")),
                    BootstrapStatus::actionFromString(
                      args.value("RECOMMENDATION"))));

      break;

    default:
      event = new UnrecognizedClientStatusEvent(severity, action, args);
  }
  dispatch(ClientStatus, event);
}

/** Parses and posts a Tor server status event. */
void
TorEvents::dispatchServerStatusEvent(tc::Severity severity,
                                     const QString &action,
                                     const QHash<QString,QString> &args)
{
  ServerStatusEvent *event;
  ServerStatusEvent::Status status
    = ServerStatusEvent::statusFromString(action);

  switch (status) {
    default:
      event = new UnrecognizedServerStatusEvent(severity, action, args);
  }
  dispatch(ServerStatus, event);
}

/** Parses and posts a general Tor status event. */
void
TorEvents::dispatchGeneralStatusEvent(tc::Severity severity,
                                      const QString &action,
                                      const QHash<QString,QString> &args)
{
  GeneralStatusEvent *event;
  GeneralStatusEvent::Status status
    = GeneralStatusEvent::statusFromString(action);

  switch (status) {
    case GeneralStatusEvent::DangerousTorVersion:
      /* Dangerous Tor version ("DANGEROUS_VERSION") */
      event = new DangerousVersionEvent(severity,
                DangerousVersionEvent::reasonFromString(args.value("REASON")),
                args.value("CURRENT"),
                args.value("RECOMMENDED").split(",", QString::SkipEmptyParts));
      break;
    default:
      event = new UnrecognizedGeneralStatusEvent(severity, action, args);
  }
  dispatch(GeneralStatus, event);
}

