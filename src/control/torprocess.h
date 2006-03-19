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
 */

#ifndef _TORPROCESS_H
#define _TORPROCESS_H

#include <QProcess>
#include <QFileInfo>
#include <QVariant>
#include <QMap>
#include <QDateTime>


class TorProcess : public QProcess
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorProcess();

  /** Start the Tor process */
  bool start(QString app, QString args, QString *errmsg = 0);
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);

  /** Returns a QString describing the last QProcess::ProcessError */
  QString errorString();

  /** Return the Tor process's PID (workaround for some Windows funkiness) */
  quint64 pid();

  /** Enable reading log messages from stdout. */
  void openStdout();
  /** Disable reading log messages from stdout. */
  void closeStdout();

signals:
  /** Emitted when Tor prints a log message to the console */
  void log(QString severity, QString message);

private slots:
  /** Called when there is data to be read from stdout */
  void onReadyRead();

private:
  /** Status of logging to stdout. */
  enum LogState {
    Open,     /**< stdout logs enabled. */
    Closing,  /**< stdout in the process of closing. */
    Closed    /**< stdout logs closed. */
  };
  /** Current state of logging on stdout. */
  LogState _logState;
  /** Timestamp of when stdout logs closed. */
  QDateTime _logCloseTime;
};

#endif

