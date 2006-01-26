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

#ifndef _TORPROCESS_H
#define _TORPROCESS_H

#include <QProcess>
#include <QFileInfo>

class TorProcess : public QProcess
{
  Q_OBJECT

public:
  TorProcess();
  ~TorProcess();

  /** Start the Tor process */
  bool start(QFileInfo app, QStringList args, QString *errmsg = 0);
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);

  /** Returns a QString describing the last QProcess::ProcessError */
  QString errorString();

  /** Return the Tor process's PID (workaround for some Windows funkiness) */
  qint64 pid();

  /** Logs messages Tor prints to stdout */
  void logStdout(bool log);

signals:
  /** Emitted when Tor prints a log message to the console */
  void log(QString severity, QString message);

private slots:
  /** Called when there is data to be read from stdout */
  void onReadyRead();

private:
  /** If _log is true, then the log() signal will be emitted when a log
   * message is written to stdout. */
  bool _log;
};

#endif

