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

#include "controlconnection.h"
#include "torprocess.h"

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
  void stop(QString *errmsg = 0);

  /** Detect if the Tor process is running */
  bool isRunning();

  /** Connect to Tor's control socket */
  bool connect(QString *errmsg = 0);

  /** Disconnect from Tor's control socket */
  void disconnect();

  /** Check if we're connected to Tor's control socket */
  bool isConnected();

  /** Sends an authentication token to Tor */
  bool authenticate(QByteArray token, QString *errmsg = 0);

  /** Sends a GETINFO message to Tor based on the given keys */
  bool getInfo(QHash<QString,QString> &map, QString *errmsg = 0);

  /** Sends a GETINFO message for a single info value to Tor */
  bool getInfo(QString key, QString &val, QString *errmsg = 0);

  /** Ask Tor for its version */
  QString getTorVersion(QString *errmsg = 0);


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
  TorProcess _torProcess;  

/* The slots below simply relay signals from the appropriate member objects */
private slots:
  void onStarted();
  void onStopped(int exitCode, QProcess::ExitStatus exitStatus);
  void onConnected();
  void onDisconnected();
};

#endif

