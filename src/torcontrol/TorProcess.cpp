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
** \file TorProcess.cpp
** \brief Starts and stops a Tor process
*/

#include "TorProcess.h"
#include "tcglobal.h"

#include "stringutil.h"

#include <QString>

/* Needed for _PROCESS_INFORMATION so that pid() works on Win32 */
#if defined (Q_OS_WIN32)
#include <qt_windows.h>
#endif


/** Default constructor */
TorProcess::TorProcess(QObject *parent)
: QProcess(parent)
{
  openStdout();
  connect(this, SIGNAL(readyReadStandardOutput()),
          this,   SLOT(onReadyRead()));
  connect(this, SIGNAL(error(QProcess::ProcessError)),
          this,   SLOT(onError(QProcess::ProcessError)));
}

/** Formats the Tor process arguments for logging. */
QString
TorProcess::formatArguments(const QStringList &args)
{
  QStringList out;
  foreach (QString arg, args) {
    out << (arg.contains(" ") || arg.isEmpty() ? string_escape(arg) : arg);
  }
  return out.join(" ");
}

/** Attempts to start the Tor process using the location, executable, and
 * command-line arguments specified in Vidalia's settings. If Tor starts, the
 * signal started() will be emitted. If Tor fails to start,
 * startFailed(errmsg) will be emitted, with an appropriate error message. */
void
TorProcess::start(const QString &app, const QStringList &args)
{
  QString exe = app;
#if defined(Q_OS_WIN32)
  /* If we're on Windows, QProcess::start requires that paths with spaces are
   * quoted before being passed to it. */
  exe = "\"" + exe + "\"";
#endif

  /* Attempt to start Tor with the given command-line arguments */
  QStringList env = QProcess::systemEnvironment();
#if !defined(Q_OS_WIN32)
  /* Add "/usr/sbin" to an existing $PATH
   * XXX What if they have no path? Would always just making one with
   *     "/usr/sbin" smart? Should we add anything else? */
  for (int i = 0; i < env.size(); i++) {
    QString envVar = env.at(i);
    if (envVar.startsWith("PATH="))
      env.replace(i, envVar += ":/usr/sbin");
  }
#endif
  setEnvironment(env);

  tc::debug("Starting Tor using '%1 %2'").arg(app).arg(formatArguments(args));
  QProcess::start(exe, args, QIODevice::ReadOnly | QIODevice::Text);
}

/** Stops the Tor process */
bool
TorProcess::stop(QString *errmsg)
{
  /* First, check if the process is already stopped before closing it
   * forcefully. */
  if (state() == QProcess::NotRunning) {
    return true;
  }

  tc::debug("Stopping the Tor process.");
  /* Tell the process to stop */
#if defined(Q_OS_WIN32)
  /* Tor on Windows doesn't understand a WM_CLOSE message (which is what
   * QProcess::terminate() sends it), so we have to kill it harshly. */
  kill();
#else
  terminate();

  /* Wait for it to complete */
  if (!waitForFinished(5000)) {
    tc::error("Tor failed to stop: %1").arg(errorString());
    if (errmsg) {
      *errmsg =
        tr("Process %1 failed to stop. [%2]").arg(pid()).arg(errorString());
    }
    return false;
  }
#endif
  return true;
}

/** Return the process ID for the current process. */
quint64
TorProcess::pid()
{
#if defined(Q_OS_WIN32)
  return (quint64)((QProcess::pid())->dwProcessId);
#else
  return QProcess::pid();
#endif
}

/** Opens logging on stdout. When this is open, the log() signal will be
 * emitted when Tor prints a message to stdout. */
void
TorProcess::openStdout()
{
  setReadChannelMode(QProcess::MergedChannels);
  setReadChannel(QProcess::StandardOutput);
}

/** Closes logging on stdout. When this is closed, the log() signal will not
 * be emitted when Tor prints a message to stdout. */
void
TorProcess::closeStdout()
{
  /* Close the stdout channel */
  closeReadChannel(QProcess::StandardOutput);
  /* Read anything left waiting on the buffer */
  onReadyRead();
}

/** Called when there is data to be read from stdout */
void
TorProcess::onReadyRead()
{
  int i, j;
  QString line;

  while (canReadLine()) {
    line = readLine();
    if (!line.isEmpty()) {
      /* Parse the log message and emit log() */
      i = line.indexOf("[");
      j = line.indexOf("]");
      if (i > 0 && j > i && line.length() >= j+2) {
        emit log(line.mid(i+1, j-i-1), line.mid(j+2));
      }
    }
  }
}

/** Called when the process encounters an error. If the error tells us that
 * the process failed to start, then we will emit the startFailed() signal and
 * an error message indicating why. */
void
TorProcess::onError(QProcess::ProcessError error)
{
  if (error == QProcess::FailedToStart) {
    tc::error("The Tor process failed to start: %1").arg(errorString());
  } else {
    tc::error("Tor process error: %1").arg(errorString());
  }
  /* Tor didn't start, so let everyone know why. */
  emit startFailed(errorString());
}

/** Returns the version reported by the Tor executable specified in
 * <b>exe</b>, or a default-constructed QString on failure. */
QString
TorProcess::version(const QString &exe)
{
  QProcess tor;

  tor.start(exe, QStringList() << "--version");
  if (!tor.waitForStarted() || !tor.waitForFinished())
    return QString();

  while (tor.canReadLine()) {
    QString line = tor.readLine();
    if (line.startsWith("Tor version", Qt::CaseInsensitive)) {
      QStringList parts = line.split(" ");
      if (parts.size() >= 3)
        return parts.at(2);
    }
  }
  return QString();
}

