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
** \file TorEvents.cpp
** \brief Parses and dispatches events from Tor
*/


#include "TorEvents.h"
#include "ControlReply.h"
#include "ReplyLine.h"
#include "Circuit.h"
#include "Stream.h"
#include "BootstrapStatus.h"

#include "stringutil.h"

#include <QHostAddress>
#include <QMetaType>

/** Format of expiry times in address map events. */
#define DATE_FMT "\"yyyy-MM-dd HH:mm:ss\""


/** Default constructor */
TorEvents::TorEvents(QObject *parent)
  : QObject(parent)
{
  qRegisterMetaType<tc::Severity>();
  qRegisterMetaType<tc::SocksError>();
  qRegisterMetaType<tc::TorVersionStatus>();

  qRegisterMetaType<BootstrapStatus>("BootstrapStatus");
  qRegisterMetaType<Circuit>("Circuit");
  qRegisterMetaType<Stream>("Stream");

  qRegisterMetaType<QHostAddress>("QHostAddress");
  qRegisterMetaType<QDateTime>("QDateTime");
}

/** Converts an event type to a string Tor understands */
QString
TorEvents::toString(Event e)
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
    case NewDescriptor:   event = "NEWDESC"; break;
    case AddressMap:      event = "ADDRMAP"; break;
    case GeneralStatus:   event = "STATUS_GENERAL"; break;
    case ClientStatus:    event = "STATUS_CLIENT"; break;
    case ServerStatus:    event = "STATUS_SERVER"; break;
    default: event = "UNKNOWN"; break;
  }
  return event;
}

/** Converts an event in the string form sent by Tor to its enum value */
TorEvents::Event
TorEvents::toTorEvent(const QString &event)
{
  Event e;
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
  } else {
    e = Unknown;
  }
  return e;
}

/** Parse the event type out of a message line and return the corresponding
 * Event enum value */
TorEvents::Event
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
    emit bandwidthUpdate(bytesIn, bytesOut);
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
      emit circuitStatusChanged(circ);
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
    Stream stream = Stream::fromString(msg.mid(i));
    if (stream.isValid())
      emit streamStatusChanged(stream);
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
  tc::Severity severity = tc::severityFromString(msg.mid(0, i));
  QString logLine = (line.getData().size() > 0 ? line.getData().join("\n") :
                                                 msg.mid(i+1));

  emit logMessage(severity, logLine);
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
  emit newDescriptors(descList);
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
    emit addressMapped(msg.at(1), msg.at(2), expires);
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
TorEvents::handleStatusEvent(Event e, const ReplyLine &line)
{
  QString status;
  tc::Severity severity;
  QHash<QString,QString> args;
  QString msg = line.getMessage();

  severity = tc::severityFromString(msg.section(' ', 1, 1));
  status   = msg.section(' ', 2, 2);
  args     = string_parse_keyvals(msg.section(' ', 3));
  switch (e) {
    case ClientStatus:
      handleClientStatusEvent(severity, status, args);
      break;

    case ServerStatus:
      handleServerStatusEvent(severity, status, args);
      break;

    case GeneralStatus:
      handleGeneralStatusEvent(severity, status, args);
      break;

    default:
      break;
  }
}

/** Parses and emits a general Tor status event. */
void
TorEvents::handleGeneralStatusEvent(tc::Severity severity,
                                    const QString &action,
                                    const QHash<QString,QString> &args)
{
  if (! action.compare("DANGEROUS_TOR_VERSION", Qt::CaseInsensitive)) {
    QString reason = args.value("REASON");
    QString current = args.value("CURRENT");
    QStringList recommended = args.value("RECOMMENDED")
                                  .split(",", QString::SkipEmptyParts);
    if (! reason.compare("NEW", Qt::CaseInsensitive))
      emit dangerousTorVersion(tc::NewTorVersion, current, recommended);
    else if (! reason.compare("UNRECOMMENDED", Qt::CaseInsensitive))
      emit dangerousTorVersion(tc::UnrecommendedTorVersion, current, recommended);
    else if (! reason.compare("OBSOLETE", Qt::CaseInsensitive)
               || ! reason.compare("OLD", Qt::CaseInsensitive))
      emit dangerousTorVersion(tc::ObsoleteTorVersion, current, recommended);
  } else if (! action.compare("CLOCK_SKEW", Qt::CaseInsensitive)) {
    int skew;
    bool ok = false;
    if (args.contains("SKEW"))
      skew = args.value("SKEW").toInt(&ok);
    else if (args.contains("MIN_SKEW"))
      skew = args.value("MIN_SKEW").toInt(&ok);
    if (ok)
      emit clockSkewed(skew, args.value("SOURCE"));
  } else if (! action.compare("BUG", Qt::CaseInsensitive)) {
    emit bug(args.value("REASON"));
  }
}

/** Parses and emits a Tor client status event. */
void
TorEvents::handleClientStatusEvent(tc::Severity severity,
                                   const QString &action,
                                   const QHash<QString,QString> &args)
{
  if (! action.compare("CIRCUIT_ESTABLISHED", Qt::CaseInsensitive)) {
    emit circuitEstablished();
  } else if (! action.compare("DANGEROUS_PORT", Qt::CaseInsensitive)) {
    bool reject = ! args.value("RESULT").compare("REJECT", Qt::CaseInsensitive);
    emit dangerousPort(args.value("PORT").toUInt(), reject);
  } else if (! action.compare("DANGEROUS_SOCKS", Qt::CaseInsensitive)) {
    emit socksError(tc::DangerousSocksTypeError, args.value("ADDRESS"));
  } else if (! action.compare("SOCKS_UNKNOWN_PROTOCOL", Qt::CaseInsensitive)) {
    emit socksError(tc::UnknownSocksProtocolError, QString());
  } else if (! action.compare("SOCKS_BAD_HOSTNAME", Qt::CaseInsensitive)) {
    emit socksError(tc::BadSocksHostnameError, args.value("HOSTNAME"));
  } else if (! action.compare("BOOTSTRAP", Qt::CaseInsensitive)) {
    BootstrapStatus status
      = BootstrapStatus(severity,
                        BootstrapStatus::statusFromString(args.value("TAG")),
                        args.value("PROGRESS").toInt(),
                        args.value("SUMMARY"),
                        args.value("WARNING"),
                        tc::connectionStatusReasonFromString(args.value("REASON")),
                        BootstrapStatus::actionFromString(
                        args.value("RECOMMENDATION")));
    emit bootstrapStatusChanged(status);
  }
}

/** Parses and emits a Tor server status event. */
void
TorEvents::handleServerStatusEvent(tc::Severity severity,
                                   const QString &action,
                                   const QHash<QString,QString> &args)
{
  if (! action.compare("EXTERNAL_ADDRESS", Qt::CaseInsensitive)) {
    emit externalAddressChanged(QHostAddress(args.value("ADDRESS")),
                                args.value("HOSTNAME"));
  } else if (! action.compare("CHECKING_REACHABILITY", Qt::CaseInsensitive)) {
    if (args.contains("ORADDRESS")) {
      QPair<QHostAddress,quint16> pair = splitAddress(args.value("ORADDRESS"));
      if (! pair.first.isNull())
        emit checkingOrPortReachability(pair.first, pair.second);
    } else if (args.contains("DIRADDRESS")) {
      QPair<QHostAddress,quint16> pair = splitAddress(args.value("DIRADDRESS"));
      if (! pair.first.isNull())
        emit checkingDirPortReachability(pair.first, pair.second);
    }
  } else if (! action.compare("REACHABILITY_SUCCEEDED", Qt::CaseInsensitive)) {
    if (args.contains("ORADDRESS")) {
      QPair<QHostAddress,quint16> pair = splitAddress(args.value("ORADDRESS"));
      if (! pair.first.isNull())
        emit orPortReachabilityFinished(pair.first, pair.second, true);
    } else if (args.contains("DIRADDRESS")) {
      QPair<QHostAddress,quint16> pair = splitAddress(args.value("DIRADDRESS"));
      if (! pair.first.isNull())
        emit dirPortReachabilityFinished(pair.first, pair.second, true);
    }
  } else if (! action.compare("REACHABILITY_FAILED", Qt::CaseInsensitive)) {
    if (args.contains("ORADDRESS")) {
      QPair<QHostAddress,quint16> pair = splitAddress(args.value("ORADDRESS"));
      if (! pair.first.isNull())
        emit orPortReachabilityFinished(pair.first, pair.second, false);
    } else if (args.contains("DIRADDRESS")) {
      QPair<QHostAddress,quint16> pair = splitAddress(args.value("DIRADDRESS"));
      if (! pair.first.isNull())
        emit dirPortReachabilityFinished(pair.first, pair.second, false);
    }
  } else if (! action.compare("GOOD_SERVER_DESCRIPTOR", Qt::CaseInsensitive)) {
    emit serverDescriptorAccepted();
  } else if (! action.compare("ACCEPTED_SERVER_DESCRIPTOR", Qt::CaseInsensitive)) {
    QPair<QHostAddress,quint16> pair = splitAddress(args.value("DIRAUTH"));
    if (! pair.first.isNull())
      emit serverDescriptorAccepted(pair.first, pair.second);
  } else if (! action.compare("BAD_SERVER_DESCRIPTOR", Qt::CaseInsensitive)) {
    QPair<QHostAddress,quint16> pair = splitAddress(args.value("DIRAUTH"));
    if (! pair.first.isNull())
      emit serverDescriptorRejected(pair.first, pair.second,
                                    args.value("REASON"));
  } else if (! action.compare("DNS_HIJACKED", Qt::CaseInsensitive)) {
    emit dnsHijacked();
  } else if (! action.compare("DNS_USELESS", Qt::CaseInsensitive)) {
    emit dnsUseless();
  }
}

/** Splits a string in the form "IP:PORT" into a QHostAddress and quint16
 * pair. If either portion is invalid, a default-constructed QPair() is
 * returned. */
QPair<QHostAddress,quint16>
TorEvents::splitAddress(const QString &address)
{
  bool ok;
  int idx = address.indexOf(":");
  if (idx <= 0 || idx >= address.length()-1)
    return QPair<QHostAddress,quint16>();

  QHostAddress ip = QHostAddress(address.mid(0, idx));
  quint16 port = static_cast<quint16>(address.mid(idx+1).toUInt(&ok));
  if (ip.isNull() || ! ok)
    return QPair<QHostAddress,quint16>();
  return QPair<QHostAddress,quint16>(ip, port);
}


