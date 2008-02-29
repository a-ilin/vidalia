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
** \file torcontrol.h
** \version $Id$
** \brief Object for interacting with the Tor process and control interface
*/

#ifndef _TORCONTROL_H
#define _TORCONTROL_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QStringList>
#include <QVariant>

#include "controlconnection.h"
#include "torprocess.h"
#include "torevents.h"
#include "torsignal.h"
#include "routerdescriptor.h"
#include "addressmap.h"
#include "protocolinfo.h"

#if defined(Q_OS_WIN32)
#include "torservice.h"
#endif


class TorControl : public QObject
{
  Q_OBJECT
  
public:
  /** Default constructor */
  TorControl();
  /** Default destructor */
  ~TorControl();

  /** Start the Tor process */
  void start(const QString &tor, const QStringList &args);
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);
  /** Detect if the Tor process is running */
  bool isRunning();
  /** Detects if the Tor process is running under Vidalia. */
  bool isVidaliaRunningTor();
  
  /** Connect to Tor's control socket */
  void connect(const QHostAddress &address, quint16 port);
  /** Disconnect from Tor's control socket */
  void disconnect();
  /** Check if we're connected to Tor's control socket */
  bool isConnected();
  /** Sends an authentication cookie to Tor. */
  bool authenticate(const QByteArray cookie, QString *errmsg = 0);
  /** Sends an authentication password to Tor. */
  bool authenticate(const QString password = QString(), QString *errmsg = 0);
  
  /** Sends a PROTOCOLINFO command to Tor and parses the response. */
  ProtocolInfo protocolInfo(QString *errmsg = 0);
 
  /** Returns true if Tor either has an open circuit or (on Tor >= 
   * 0.2.0.1-alpha) has previously decided it's able to establish a circuit. */
  bool circuitEstablished();

  /** Sends a GETINFO message to Tor based on the given keys */
  bool getInfo(QHash<QString,QString> &map, QString *errmsg = 0);
  /** Sends a GETINFO message for a single info value to Tor */
  bool getInfo(QString key, QString &val, QString *errmsg = 0);

  /** Sends a GETINFO message to Tor using the given list of <b>keys</b> and
   * returns a QVariantMap containing the specified keys and their values as
   * returned by Tor. Returns a default constructed QVariantMap on failure. */
  QVariantMap getInfo(const QStringList &keys, QString *errmsg = 0);
  /** Sends a GETINFO message to Tor with a single <b>key</b> and returns a
   * QVariant containing the value returned by Tor. Returns a default
   * constructed QVariant on failure. */
  QVariant getInfo(const QString &key, QString *errmsg = 0);


  /** Sends a signal to Tor */
  bool signal(TorSignal::Signal sig, QString *errmsg = 0);
 
  /** Returns an address on which Tor is listening for application
   * requests. If none are available, a null QHostAddress is returned. */
  QHostAddress getSocksAddress(QString *errmsg = 0);
  /** Returns a (possibly empty) list of all currently configured 
   * SocksListenAddress entries. */
  QStringList getSocksAddressList(QString *errmsg = 0);
  /** Returns a valid SOCKS port for Tor, or 0 if Tor is not accepting
   * application requests. */
  quint16 getSocksPort(QString *errmsg = 0);
  /** Returns a list of all currently configured SOCKS ports. If Tor is not
   * accepting any application connections, an empty list will be returned. */
  QList<quint16> getSocksPortList(QString *errmsg = 0);

  /** Returns Tor's version as a string. */
  QString getTorVersionString();
  /** Returns Tor's version as a numeric value. */
  quint32 getTorVersion();

  /** Sets an event and its handler. If add is true, then the event is added,
   * otherwise it is removed. If set is true, then the given event will be
   * registered with Tor. */
  bool setEvent(TorEvents::TorEvent e, QObject *obj, 
                bool add, bool set = true, QString *errmsg = 0);
  /** Registers for a set of logging events according to the given filter. */
  bool setLogEvents(uint filter, QObject *obj, QString *errmsg = 0);
  /** Register events of interest with Tor */
  bool setEvents(QString *errmsg = 0);
  

  /** Sets each configuration key in <b>map</b> to the value associated with its key. */
  bool setConf(QHash<QString,QString> map, QString *errmsg = 0);
  /** Sets a single configuration key to the given value. */
  bool setConf(QString key, QString value, QString *errmsg = 0);
  /** Gets values for a set of configuration keys, each of which has a single
   * value. */
  bool getConf(QHash<QString,QString> &map, QString *errmsg = 0);
  /** Gets a set of configuration keyvalues and stores them in <b>map</b>. */
  bool getConf(QHash<QString,QStringList> &map, QString *errmsg = 0);
  /** Gets a single configuration value for <b>key</b>. */
  bool getConf(QString key, QString &value, QString *errmsg = 0);
  /** Gets a list of configuration values for <b>key</b>. */
  bool getConf(QString key, QStringList &value, QString *errmsg = 0);

  /** Sends a GETCONF message to Tor using the given list of <b>keys</b> and
   * returns a QVariantMap containing the specified keys and their values as
   * returned by Tor. Returns a default constructed QVariantMap on failure. */
  QVariantMap getConf(const QStringList &keys, QString *errmsg = 0);
  /** Sends a GETCONF message to Tor with a single <b>key</b> and returns a
   * QVariant containing the value returned by Tor. Returns a default
   * constructed QVariant on failure. */
  QVariant getConf(const QString &key, QString *errmsg = 0);
  
  /** Asks Tor to save the current configuration to its torrc */
  bool saveConf(QString *errmsg = 0);
  /** Tells Tor to reset the given configuration keys back to defaults. */
  bool resetConf(QStringList keys, QString *errmsg = 0);
  /** Tells Tor to reset a configuration key back to its default value. */
  bool resetConf(QString key, QString *errmsg = 0);

  /** Gets a descriptor for the given router name. */
  RouterDescriptor getDescriptorByName(QString name, QString *errmsg = 0);
  /** Gets a descriptor for the given router ID. */
  RouterDescriptor getDescriptorById(QString id, QString *errmsg = 0);
  /** Gets descriptors for the given list of router names. */
  QList<RouterDescriptor> getDescriptorListByName(QStringList names, QString *errmsg = 0);
  /** Gets descriptors for the given list of router IDs. */
  QList<RouterDescriptor> getDescriptorListById(QStringList ids, QString *errmsg = 0);
  /** Gets a list of descriptors for all routers Tor knows about. */
  QList<RouterDescriptor> getRouterList(QString *errmsg = 0);
  /** Gets a list of router IDs for all routers Tor knows about. */
  QStringList getRouterIDList(QString *errmsg = 0);

  /** Gets a list of current circuits. */
  QList<Circuit> getCircuits(QString *errmsg = 0);
  /** Gets a list of current streams. */
  QList<Stream> getStreams(QString *errmsg = 0);
  
  /** Gets a list of address mappings of the type specified by <b>type</b>
   * (defaults to <i>AddressMapAll</i>. */
  AddressMap getAddressMap(
    AddressMap::AddressMapType type = AddressMap::AddressMapAll,
    QString *errmsg = 0);

public slots:
  /** Closes the circuit specified by <b>circid</b>. If <b>ifUnused</b> is
   * true, then the circuit will not be closed unless it is unused. */
  bool closeCircuit(quint64 circid, bool ifUnused = false, QString *errmsg = 0);
  /** Closes the stream specified by <b>streamid</b>. */
  bool closeStream(quint64 streamid, QString *errmsg = 0);

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

private:
  /** Instantiates a connection used to talk to Tor's control port */
  ControlConnection* _controlConn;
  /** Manages and monitors the Tor process */
  TorProcess* _torProcess;
  /** Keep track of which events we're interested in */
  TorEvents _torEvents;
  /** The version of Tor we're currently talking to. */
  QString _torVersion;
#if defined(Q_OS_WIN32)
  /** Manages the Tor service, if supported and enabled */
  TorService* _torService;
#endif

  /** Send a message to Tor and read the response */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);
  /** Send a message to Tor and discard the response */
  bool send(ControlCommand cmd, QString *errmsg = 0);
  
/* The slots below simply relay signals from the appropriate member objects */
private slots:
  void onStarted();
  void onStartFailed(QString errmsg);
  void onStopped(int exitCode, QProcess::ExitStatus exitStatus);
  void onConnected();
  void onConnectFailed(QString errmsg);
  void onDisconnected();
  void onLogStdout(QString severity, QString message);
  void onAuthenticated();
};

#endif

