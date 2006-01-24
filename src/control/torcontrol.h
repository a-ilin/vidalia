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

#ifndef _TORCONTROL_H
#define _TORCONTROL_H

#include <QObject>
#include <QHash>
#include <QList>

#include "controlconnection.h"
#include "torprocess.h"
#include "messagepump.h"
#include "torevents.h"


class TorControl : public QObject
{
  Q_OBJECT
  
public:
  /** Signals that can be sent by the controller */
  enum Signal {
    SignalReload, SignalShutdown, SignalDump, SignalDebug, SignalHalt
  };
 

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
  bool connect(QString *errmsg = 0);
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
  bool signal(Signal sig, QString *errmsg = 0);
  
  /** Ask Tor for its version */
  QString getTorVersion(QString *errmsg = 0);

  /** Register another event of interest with Tor */
  bool addEvent(TorEvents::TorEvent e, QObject *obj, QString *errmsg = 0);
  /** Remove a previously registered event */
  bool removeEvent(TorEvents::TorEvent e, QObject *obj, QString *errmsg = 0);
  /** Register events of interest with Tor */
  bool setEvents(QString *errmsg = 0);
  

signals:
  /** Emitted when the Tor process has started */
  void started();
  /** Emitted when the Tor process has stopped */
  void stopped(int exitCode, QProcess::ExitStatus exitStatus);
  /** Emitted when the controller has connected to Tor */
  void connected();
  /** Emitted when the controller has disconnected from Tor */
  void disconnected();

  
private:
  /** Instantiates a socket used to connect to Tor's control port */
  ControlConnection _controlConn;
  /** Manages and monitors the Tor process */
  TorProcess *_torProcess;  
  /** Handles sending and receiving messages from Tor over the control
   * connection */
  MessagePump *_messages;
  /** Keep track of which events we're interested in */
  TorEvents _torEvents;

  /** Send a message to Tor and read the response */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);
  
  
/* The slots below simply relay signals from the appropriate member objects */
private slots:
  void onStarted();
  void onStopped(int exitCode, QProcess::ExitStatus exitStatus);
  void onConnected();
  void onDisconnected();
};

#endif

