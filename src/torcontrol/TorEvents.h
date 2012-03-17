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
** \file TorEvents.h
** \brief Parses and dispatches events from Tor
*/

#ifndef _TOREVENTS_H
#define _TOREVENTS_H

#include "tcglobal.h"

#include <QObject>
#include <QMultiHash>
#include <QList>
#include <QFlags>

class Circuit;
class Stream;
class BootstrapStatus;
class ControlReply;
class ReplyLine;

class QString;
class QDateTime;
class QHostAddress;


class TorEvents : public QObject
{
  Q_OBJECT

public:
  /** Asynchronous events sent from Tor to the controller */
  enum Event {
    Unknown       = 0,
    Bandwidth     = (1u << 0),
    LogDebug      = (1u << 1),
    LogInfo       = (1u << 2),
    LogNotice     = (1u << 3),
    LogWarn       = (1u << 4),
    LogError      = (1u << 5),
    CircuitStatus = (1u << 6),
    StreamStatus  = (1u << 7),
    OrConnStatus  = (1u << 8),
    NewDescriptor = (1u << 9),
    AddressMap    = (1u << 10),
    GeneralStatus = (1u << 11),
    ClientStatus  = (1u << 12),
    ServerStatus  = (1u << 13)
  };
  static const Event EVENT_MIN = TorEvents::Bandwidth;
  static const Event EVENT_MAX = TorEvents::ServerStatus;
  Q_DECLARE_FLAGS(Events, Event);

  /** Default Constructor */
  TorEvents(QObject *parent = 0);

  /** Parses an event message and emits the proper signal */
  void handleEvent(const ControlReply &reply);

  /** Converts an Event to a string */
  static QString toString(TorEvents::Event e);

signals:
  /** Emitted when Tor writes the message <b>msg</b> to the control port
   * with message severity <b>level</b>.
   */
  void logMessage(tc::Severity level, const QString &msg);

  /** Emitted when Tor sends a bandwidth usage update (roughly once every
   * second). <b>bytesReceived</b> is the number of bytes read by Tor over
   * the previous second and <b>bytesWritten</b> is the number of bytes
   * sent over the same interval.
   */
  void bandwidthUpdate(quint64 bytesReceived, quint64 bytesSent);

  /** Emitted when the stream status of <b>stream</b> has changed.
   */
  void streamStatusChanged(const Stream &stream);

  /** Emitted when the circuit status of <b>circuit</b> has changed.
   */
  void circuitStatusChanged(const Circuit &circuit);

  /** Emitted when Tor has mapped the address <b>from</b> to the address
   * <b>to</b>. <b>expires</b> indicates the time at which when the address
   * mapping will no longer be considered valid.
   */
  void addressMapped(const QString &from, const QString &to,
                     const QDateTime &expires);

  /** Emitted when Tor has received one or more new router descriptors.
   * <b>ids</b> contains a list of digests of the new descriptors.
   */
  void newDescriptors(const QStringList &ids);

  /** Indicates Tor has been able to successfully establish one or more
   * circuits.
   */
  void circuitEstablished();

  /** Indicates that Tor has decided the user's Tor software <b>version</b>
   * is no longer recommended for some <b>reason</b>. <b>recommended</b> is
   * a list of Tor software versions that are considered current.
   */
  void dangerousTorVersion(tc::TorVersionStatus reason,
                           const QString &version,
                           const QStringList &recommended);

  /** Emitted during Tor's startup process to indicate how far in its
   * bootstrapping process it has progressed. <b>status</b> may indicate
   * the current bootstrapping stage or an error during bootstrapping.
   */
  void bootstrapStatusChanged(const BootstrapStatus &status);

  /** Emitted when the user attempts to establish a connection to some
   * destination on port <b>port</b>, which is a port known to use
   * plaintext connections (as determined by Tor's WarnPlaintextPorts and
   * RejectPlaintextPorts torrc options). <b>rejected</b> indicates whether
   * Tor rejected the connection or permitted it to connect anyway.
   */
  void dangerousPort(quint16 port, bool rejected);

  /** Emitted when Tor detects a problem with a SOCKS connection from the
   * user, such as a bad hostname, dangerous SOCKS protocol type, or a bad
   * hostname. <b>type</b> indicates the type of error encountered and
   * <b>destination</b> (if non-empty) specifies the attempted connection
   * destination address or hostname.
   */
  void socksError(tc::SocksError error, const QString &destination);

  /** Emitted when Tor has encountered an internal bug. <b>reason</b> is
   * Tor's description of the bug.
   */
  void bug(const QString &reason);

  /** Emitted when Tor decides the client's external IP address has changed
   * to <b>ip</b>. If <b>hostname</b> is non-empty, Tor obtained the new
   * value for <b>ip</b> by resolving <b>hostname</b>.
   */
  void externalAddressChanged(const QHostAddress &ip, const QString &hostname);

  /** Indicates that Tor has determined the client's clock is potentially
   * skewed by <b>skew</b> seconds relative to <b>source</b>.
   */
  void clockSkewed(int skew, const QString &source);

  /** Emitted when Tor determines that the user's DNS provider is providing
   * an address for non-existent domains when it should really be saying
   * "NXDOMAIN".
   */
  void dnsHijacked();

  /** Emitted when Tor determines that the user's DNS provider is providing
   * a hijacked address even for well-known websites.
   */
  void dnsUseless();

  /** Indicates Tor has started testing the reachability of its OR port
   * using the IP address <b>ip</b> and port <b>port</b>.
   */
  void checkingOrPortReachability(const QHostAddress &ip, quint16 port);

  /** Tor has completed testing the reachability of its OR port using
   * the IP address <b>ip</b> and port <b>port</b>. If the user's OR port
   * was reachable, <b>reachable</b> will be set to true.
   */
  void orPortReachabilityFinished(const QHostAddress &ip, quint16 port,
                                  bool reachable);

  /** Indicates Tor has started testing the reachability of its directory
   * port using the IP address <b>ip</b> and port <b>port</b>.
   */
  void checkingDirPortReachability(const QHostAddress &ip, quint16 port);

  /** Tor has completed testing the reachability of its directory port using
   * the IP address <b>ip</b> and port <b>port</b>. If the user's directory
   * port was reachable, <b>reachable</b> will be set to true.
   */
  void dirPortReachabilityFinished(const QHostAddress &ip, quint16 port,
                                   bool reachable);

  /** Emitted when the directory authority with IP address <b>ip</b> and
   * port <b>port</b> rejected the user's server descriptor. <b>reason</b>
   * describes why the descriptor was rejected (e.g., malformed, skewed
   * clock, etc.).
   */
  void serverDescriptorRejected(const QHostAddress &ip, quint16 port,
                                const QString &reason);

  /** Emitted when the directory authority with IP address <b>ip</b> and
   * port <b>port</b> accepted the user's server descriptor.
   */
  void serverDescriptorAccepted(const QHostAddress &ip, quint16 port);

  /** Emitted when at least one directory authority has accepted the user's
   * server descriptor.
   */
  void serverDescriptorAccepted();

private:
  /** Parses the event type from the event message */
  static Event parseEventType(const ReplyLine &line);
  /** Converts a string to an Event */
  static Event toTorEvent(const QString &event);
  /** Splits a string in the form "IP:PORT" into a QHostAddress and quint16
   * pair. If either portion is invalid, a default-constructed QPair() is
   * returned. */
   static QPair<QHostAddress,quint16> splitAddress(const QString &address);

  /** Handle a bandwidth update event */
  void handleBandwidthUpdate(const ReplyLine &line);
  /** Handle a circuit status event */
  void handleCircuitStatus(const ReplyLine &line);
  /** Handle a stream status event */
  void handleStreamStatus(const ReplyLine &line);
  /** Handle a log message event */
  void handleLogMessage(const ReplyLine &line);
  /** Handle an OR connection status event. */
  void handleOrConnStatus(const ReplyLine &line);
  /** Handles a new list of descriptors event. */
  void handleNewDescriptor(const ReplyLine &line);
  /** Handles a new or updated address map event. */
  void handleAddressMap(const ReplyLine &line);

  /** Handles a Tor status event. */
  void handleStatusEvent(Event type, const ReplyLine &line);
  /** Parses and posts a general Tor status event. */
  void handleGeneralStatusEvent(tc::Severity severity,
                                const QString &action,
                                const QHash<QString,QString> &args);
  /** Parses and posts a Tor client status event. */
  void handleClientStatusEvent(tc::Severity severity,
                               const QString &action,
                               const QHash<QString,QString> &args);
  /** Parses and posts a Tor server status event. */
  void handleServerStatusEvent(tc::Severity severity,
                               const QString &action,
                               const QHash<QString,QString> &args);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TorEvents::Events)

#endif

