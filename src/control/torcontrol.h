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
 * \file torcontrol.h
 * \version $Id$
 */

#ifndef _TORCONTROL_H
#define _TORCONTROL_H

#include <QObject>
#include <QHash>
#include <QList>

#include "controlconnection.h"
#include "torprocess.h"
#include "torevents.h"
#include "torsignal.h"
#include "routerdescriptor.h"


class TorControl : public QObject
{
  Q_OBJECT
  
public:
  /** Default constructor */
  TorControl();
  /** Default destructor */
  ~TorControl();

  /** Start the Tor process */
  bool start(QString *errmsg = 0);
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);
  /** Detect if the Tor process is running */
  bool isRunning();

  /** Connect to Tor's control socket */
  void connect();
  /** Disconnect from Tor's control socket */
  void disconnect();
  /** Check if we're connected to Tor's control socket */
  bool isConnected();
  /** Sends an authentication token to Tor */
  bool authenticate(QString *errmsg = 0);

  /** Sends a GETINFO message to Tor based on the given keys */
  bool getInfo(QHash<QString,QString> &map, QString *errmsg = 0);
  /** Sends a GETINFO message for a single info value to Tor */
  bool getInfo(QString key, QString &val, QString *errmsg = 0);

  /** Sends a signal to Tor */
  bool signal(TorSignal::Signal sig, QString *errmsg = 0);
  
  /** Ask Tor for its version */
  QString getTorVersion(QString *errmsg = 0);

  /** Sets an event and its handler. If add is true, then the event is added,
   * otherwise it is removed. If set is true, then the given event will be
   * registered with Tor. */
  bool setEvent(TorEvents::TorEvent e, QObject *obj, 
                bool add, bool set = true, QString *errmsg = 0);
  /** Registers for a set of logging events according to the given filter. */
  bool setLogEvents(uint filter, QObject *obj, QString *errmsg = 0);
  /** Register events of interest with Tor */
  bool setEvents(QString *errmsg = 0);
  

  /** Sets each configuration key in \emph map to the value associated with its key. */
  bool setConf(QHash<QString,QString> map, QString *errmsg = 0);
  /** Sets a single configuration key to the given value. */
  bool setConf(QString key, QString value, QString *errmsg = 0);
  /** Gets a set of configuration keyvalues and stores them in \emph map. */
  bool getConf(QHash<QString,QString> &map, QString *errmsg = 0);
  /** Gets a single configuration keyvalue. */
  bool getConf(QString key, QString &value, QString *errmsg = 0);
  /** Asks Tor to save the current configuration to its torrc */
  bool saveConf(QString *errmsg = 0);
  /** Tells Tor to reset the given configuration keys back to defaults. */
  bool resetConf(QStringList keys, QString *errmsg = 0);
  /** Tells Tor to reset a configuration key back to its default value. */
  bool resetConf(QString key, QString *errmsg = 0);

  /** Gets a descriptor for the given router ID. */
  RouterDescriptor getRouterDescriptor(QString id, QString *errmsg = 0);
  /** Gets a list of descriptors for all routers Tor knows about. */
  QList<RouterDescriptor> getRouterList(QString *errmsg = 0);
  /** Gets a list of router IDs for all routers Tor knows about. */
  QStringList getRouterIDList(QString *errmsg = 0);

signals:
  /** Emitted when the Tor process has started */
  void started();
  /** Emitted when the Tor process has stopped */
  void stopped(int exitCode, QProcess::ExitStatus exitStatus);
  /** Emitted when the controller has connected to Tor */
  void connected();
  /** Emitted when the controller failed to connect to Tor. */
  void connectFailed(QString errmsg);
  /** Emitted when the controller has disconnected from Tor */
  void disconnected();
  /** Emitted when the connection status changes. */
  void connected(bool connected);

private:
  /** Instantiates a connection used to talk to Tor's control port */
  ControlConnection* _controlConn;
  /** Manages and monitors the Tor process */
  TorProcess *_torProcess;  
  /** Keep track of which events we're interested in */
  TorEvents _torEvents;

  /** Send a message to Tor and read the response */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);
  /** Send a message to Tor and discard the response */
  bool send(ControlCommand cmd, QString *errmsg = 0);
  /** Disconnects signals from the TorProcess and frees its memory. */
  void closeTorProcess();
  
/* The slots below simply relay signals from the appropriate member objects */
private slots:
  void onStarted();
  void onStopped(int exitCode, QProcess::ExitStatus exitStatus);
  void onConnected();
  void onConnectFailed(QString errmsg);
  void onDisconnected();
  void onLogStdout(QString severity, QString message);
};

#endif

