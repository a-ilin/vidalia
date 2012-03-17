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
** \file TorProcess.h
** \brief Starts and stops a Tor process
*/

#ifndef _TORPROCESS_H
#define _TORPROCESS_H

#include <QProcess>


class TorProcess : public QProcess
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorProcess(QObject *parent = 0);

  /** Start the Tor process */
  void start(const QString &app, const QStringList &args);
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);

  /** Return the Tor process's PID (workaround for some Windows funkiness) */
  quint64 pid();

  /** Enable reading log messages from stdout. */
  void openStdout();
  /** Disable reading log messages from stdout. */
  void closeStdout();

  /** Returns the version reported by the Tor executable specified in
   * <b>exe</b>, or a default-constructed QString on failure. */
  static QString version(const QString &exe);

signals:
  /** Emitted when Tor prints a log message to the console */
  void log(const QString &severity, const QString &message);
  /** Emitted when Tor fails to start, perhaps because the path to Tor was
   * bogus. */
  void startFailed(const QString &errorMessage);

private slots:
  /** Called when there is data to be read from stdout */
  void onReadyRead();
  /** Called when an error occurs in the process. */
  void onError(QProcess::ProcessError error);

private:
  /** Formats the Tor process arguments for logging. */
  QString formatArguments(const QStringList &args);
};

#endif

