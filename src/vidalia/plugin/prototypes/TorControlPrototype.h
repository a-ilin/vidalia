/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/*
** \file TorControlPrototype.h
** \brief Prototype for TorControl class
*/

#ifndef _TORCONTROLPROTO_H
#define _TORCONTROLPROTO_H

#include <QtGui>
#include <QtScript>

#include "TorControl.h"
#include "ProtocolInfo.h"

class TorControlPrototype : public QObject, QScriptable
{
  Q_OBJECT
  
public:
  TorControlPrototype();

  static int metaTypeId();
  static QString name();

  /** Start the Tor process */
  Q_INVOKABLE void start(const QString &tor, const QStringList &args);
  /** Stop the Tor process */
  Q_INVOKABLE QVariant stop();
  /** Detect if the Tor process is running */
  Q_INVOKABLE bool isRunning();
  /** Detects if the Tor process is running under Vidalia. */
  Q_INVOKABLE bool isVidaliaRunningTor();
  /** Stops reading log messages from the Tor process's stdout. This has no
   * effect if isVidaliaRunningTor() is false. */
  Q_INVOKABLE void closeTorStdout();

  /** Connect to Tor's control socket */
  Q_INVOKABLE void connect(const QHostAddress &address, quint16 port);
  Q_INVOKABLE void connect(const QString &path);
  /** Disconnect from Tor's control socket */
  Q_INVOKABLE void disconnect();
  /** Check if we're connected to Tor's control socket */
  Q_INVOKABLE bool isConnected();
  /** Sends an authentication cookie to Tor. */
  Q_INVOKABLE QVariant authenticate(const QByteArray cookie);
  /** Sends an authentication password to Tor. */
  Q_INVOKABLE QVariant authenticate(const QString &password = QString());
  
  /** Sends a PROTOCOLINFO command to Tor and parses the response. */
//  Q_INVOKABLE QVariant protocolInfo();

  /** Returns the Tor software's current bootstrap phase and status. */
//  Q_INVOKABLE BootstrapStatus bootstrapStatus(QString *errmsg = 0);

  /** Returns true if Tor either has an open circuit or (on Tor >= 
   * 0.2.0.1-alpha) has previously decided it's able to establish a circuit. */
  Q_INVOKABLE bool isCircuitEstablished();

  /** Sends a GETINFO message to Tor based on the given keys */
  Q_INVOKABLE QVariant getInfo(QHash<QString,QString> &map);
  /** Sends a GETINFO message for a single info value to Tor */
//  Q_INVOKABLE QVariant getInfo(QString key);

  /** Sends a GETINFO message to Tor using the given list of <b>keys</b> and
   * returns a QVariantMap containing the specified keys and their values as
   * returned by Tor. Returns a default constructed QVariantMap on failure. */
//  Q_INVOKABLE QVariantMap getInfo(const QStringList &keys, QString *errmsg = 0);
  /** Sends a GETINFO message to Tor with a single <b>key</b> and returns a
   * QVariant containing the value returned by Tor. Returns a default
   * constructed QVariant on failure. */
  Q_INVOKABLE QVariant getInfo(const QString &key);

  /** Sends a signal to Tor */
  Q_INVOKABLE QVariant signal(TorSignal::Signal sig);
 
  /** Returns an address on which Tor is listening for application
   * requests. If none are available, a null QHostAddress is returned. */
//  Q_INVOKABLE QVariant getSocksAddress();
  /** Returns a (possibly empty) list of all currently configured 
   * SocksListenAddress entries. */
  Q_INVOKABLE QStringList getSocksAddressList(QString *errmsg = 0);
  /** Returns a valid SOCKS port for Tor, or 0 if Tor is not accepting
   * application requests. */
  Q_INVOKABLE QVariant getSocksPort();
  /** Returns a list of all currently configured SOCKS ports. If Tor is not
   * accepting any application connections, an empty list will be returned. */
  Q_INVOKABLE QList<quint16> getSocksPortList(QString *errmsg = 0);

  /** Returns Tor's version as a string. */
  Q_INVOKABLE QString getTorVersionString();
  /** Returns Tor's version as a numeric value. */
  Q_INVOKABLE quint32 getTorVersion();

  /** Sets an event and its handler. If add is true, then the event is added,
   * otherwise it is removed. If set is true, then the given event will be
   * registered with Tor. */
  Q_INVOKABLE QVariant setEvent(TorEvents::Event e, bool add = true, bool set = true);
  /** Register events of interest with Tor */
  Q_INVOKABLE QVariant setEvents();

  /** Sets each configuration key in <b>map</b> to the value associated with its key. */
  Q_INVOKABLE QVariant setConf(QHash<QString,QString> map);
  /** Sets a single configuration key to the given value. */
  Q_INVOKABLE QVariant setConf(QString key, QString value);
  /** Sets a single configuration string that is formatted <key=escaped value>. */
  Q_INVOKABLE QVariant setConf(QString keyAndValue);
  /** Gets values for a set of configuration keys, each of which has a single
   * value. */
  Q_INVOKABLE bool getConf(QHash<QString,QString> &map, QString *errmsg);
  /** Gets a set of configuration keyvalues and stores them in <b>map</b>. */
  Q_INVOKABLE bool getConf(QHash<QString,QStringList> &map, QString *errmsg = 0);
  /** Gets a single configuration value for <b>key</b>. */
  Q_INVOKABLE QVariant getConf(QString key);
  /** Gets a list of configuration values for <b>key</b>. */
  Q_INVOKABLE bool getConf(QString key, QStringList &value, QString *errmsg = 0);

  /** Sends a GETCONF message to Tor using the given list of <b>keys</b> and
   * returns a QVariantMap containing the specified keys and their values as
   * returned by Tor. Returns a default constructed QVariantMap on failure. */
  Q_INVOKABLE QVariantMap getConf(const QStringList &keys, QString *errmsg = 0);
  /** Sends a GETCONF message to Tor with a single <b>key</b> and returns a
   * QVariant containing the value returned by Tor. Returns a default
   * constructed QVariant on failure. */
//  Q_INVOKABLE QVariant getConf(const QString &key, QString *errmsg = 0);
  /** Sends a GETCONF message to Tor with the single key and returns a QString
   * containing the value returned by Tor */
  Q_INVOKABLE QVariant getHiddenServiceConf(const QString &key);
  
  /** Asks Tor to save the current configuration to its torrc */
  Q_INVOKABLE QVariant saveConf();
  /** Tells Tor to reset the given configuration keys back to defaults. */
  Q_INVOKABLE bool resetConf(QStringList keys, QString *errmsg = 0);
  /** Tells Tor to reset a configuration key back to its default value. */
  Q_INVOKABLE QVariant resetConf(QString key);

  /** Returns an unparsed router descriptor for the router whose fingerprint
   * matches <b>id</b>. The returned text can later be parsed by the
   * RouterDescriptor class. If <b>id</b> is invalid, then an empty
   * QStringList is returned. */
  Q_INVOKABLE QStringList getRouterDescriptorText(const QString &id, QString *errmsg = 0);
  /** Returns the descriptor for the router whose fingerprint matches
   * <b>id</b>. If <b>id</b> is invalid or the router's descriptor cannot be
   * parsed, then an invalid RouterDescriptor is returned. */
  Q_INVOKABLE RouterDescriptor getRouterDescriptor(const QString &id, QString *errmsg = 0);
  /** Returns the status of the router whose fingerprint matches <b>id</b>. If
   * <b>id</b> is invalid or the router's status cannot be parsed, then an
   * invalid RouterStatus is returned. */
  Q_INVOKABLE RouterStatus getRouterStatus(const QString &id, QString *errmsg = 0);
  /** Returns a RouterStatus object for every known router in the network. If
   * the network status document cannot be parsed, then an empty NetworkStatus
   * is returned. */
  Q_INVOKABLE NetworkStatus getNetworkStatus(QString *errmsg = 0);
  /** Returns the annotations for the router whose fingerprint matches
   * <b>id</b>. If <b>id</b> is invalid or the router's descriptor cannot be
   * parsed, then an empty DescriptorAnnotations is returned and
   * <b>errmsg</b> is set if it's not NULL. (Tor >= 0.2.0.13-alpha only) */
  Q_INVOKABLE DescriptorAnnotations getDescriptorAnnotations(const QString &id,
                                                 QString *errmsg = 0);

  /** Gets a list of current circuits. */
  Q_INVOKABLE CircuitList getCircuits(QString *errmsg = 0);
  /** Gets a list of current streams. */
  Q_INVOKABLE StreamList getStreams(QString *errmsg = 0);
  
  /** Gets a list of address mappings of the type specified by <b>type</b>
   * (defaults to <i>AddressMapAll</i>. */
  Q_INVOKABLE AddressMap getAddressMap(
    AddressMap::AddressMapType type = AddressMap::AddressMapAll,
    QString *errmsg = 0);

  /** Gets the ISO-3166 two-letter country code for <b>ip</b> from Tor.
   * Returns a default-constructed QString on failure or if a country code
   * is not known for <b>ip</b>. On failure, <b>errmsg</b> will be set if
   * it's not NULL. */
  Q_INVOKABLE QVariant ipToCountry(const QHostAddress &ip);

public slots:
  /** Closes the circuit specified by <b>circId</b>. If <b>ifUnused</b> is
   * true, then the circuit will not be closed unless it is unused. */
  bool closeCircuit(const CircuitId &circId, bool ifUnused = false,
                    QString *errmsg = 0);
  /** Closes the stream specified by <b>streamId</b>. */
  bool closeStream(const StreamId &streamId, QString *errmsg = 0);

signals:
  /** Emitted when the Tor process has started */
  void started();
  /** Emitted when the Tor process fails to start. */
  void startFailed(QString errmsg);
  /** Emitted when the Tor process has stopped */
  void stopped(int exitCode, QProcess::ExitStatus exitStatus);
  /** Emitted when the Tor process has stopped. */
  void stopped();
  /** Emitted when the controller has connected to Tor */
  void connected();
  /** Emitted when the controller failed to connect to Tor. */
  void connectFailed(QString errmsg);
  /** Emitted when the controller has disconnected from Tor */
  void disconnected();
  /** Emitted when the control socket is connected and authenticated. */
  void authenticated();
  /** Emitted when Tor rejects our authentication attempt. */
  void authenticationFailed(QString errmsg);

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
  void socksError(tc::SocksError type, const QString &destination);

  /** Emitted when Tor decides the client's external IP address has changed
   * to <b>ip</b>. If <b>hostname</b> is non-empty, Tor obtained the new
   * value for <b>ip</b> by resolving <b>hostname</b>. 
   */
  void externalAddressChanged(const QHostAddress &ip, const QString &hostname);

  /** Indicates that Tor has determined the client's clock is potentially
   * skewed by <b>skew</b> seconds relative to <b>source</b>.
   */
  void clockSkewed(int skew, const QString &source);

  /** Emitted when Tor has encountered an internal bug. <b>reason</b> is
   * Tor's description of the bug.
   */
  void bug(const QString &reason);

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
};

Q_DECLARE_METATYPE(TorControl *);

#endif


