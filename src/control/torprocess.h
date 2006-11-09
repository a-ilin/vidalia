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
 * \file torprocess.h
 * \version $Id$
 * \brief Starts and stops a Tor process
 */

#ifndef _TORPROCESS_H
#define _TORPROCESS_H

#include <QProcess>


class TorProcess : public QProcess
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorProcess();

  /** Start the Tor process */
  void start(QString app, QString args);
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);

  /** Return the Tor process's PID (workaround for some Windows funkiness) */
  quint64 pid();

  /** Enable reading log messages from stdout. */
  void openStdout();
  /** Disable reading log messages from stdout. */
  void closeStdout();

signals:
  /** Emitted when Tor prints a log message to the console */
  void log(QString severity, QString message);
  /** Emitted when Tor fails to start, perhaps because the path to Tor was
   * bogus. */
  void startFailed(QString errorMessage);
  
private slots:
  /** Called when there is data to be read from stdout */
  void onReadyRead();
  /** Called when an error occurs in the process. */
  void onError(QProcess::ProcessError error);
};

#endif

