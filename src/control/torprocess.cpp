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

#include <QFileInfo>
#include <QCoreApplication>


/* Needed for _PROCESS_INFORMATION so that pid() works on Win32 */
#if defined (Q_OS_WIN32)
#include <windows.h>
#endif
  
#include "torprocess.h"

/** Default constructor */
TorProcess::TorProcess()
{
  _log = true;
  setReadChannelMode(QProcess::MergedChannels);
  QObject::connect(this, SIGNAL(readyRead()), 
                   this, SLOT(onReadyRead()), Qt::DirectConnection);
}

/** Destructor */
TorProcess::~TorProcess()
{
  /* We're being destroyed. Don't read stdout anymore and flush any pending
   * events. */
  logStdout(false);
}

/** Attempts to start the Tor process using the location, executable, and
 * command-line arguments specified in Vidalia's settings. If Tor doesn't
 * exist at the given path, <b>errmsg</b> will be set appropriately and the
 * function will return false. */
bool TorProcess::start(QString app, QStringList args, QString *errmsg) 
{
  QFileInfo appInfo(app);
  QString path = appInfo.absoluteFilePath();
  QString exec;
  
  /* If the path doesn't point to an executable, then bail */
  if (!appInfo.isExecutable()) {
    if (errmsg) {
      *errmsg = 
        QString("\"%1\" is not an executable.").arg(path);
    }
    return false;
  }
  
  /* This lovely little hack here is because the isExecutable() method above
   * doesn't understand quoted paths, but the QProcess::start() method below
   * requires it, on Windows. */
#if defined(Q_OS_WIN32)
  path = "\"" + path + "\"";
#endif

  /* If I simply pass the QStringList of command-line arguments to
   * QProcess::start(path, args), then Qt will quote any arguments with spaces
   * in them. This is logical, but Tor doesn't like it because you end up with
   * something like "-ControlPort 9051", which Tor thinks is a single
   * argument. So I'll just do the joining myself. */
  exec = path + " " + args.join(" ");
  
  /* Attempt to start Tor with the given command-line arguments */
  QProcess::start(exec, QIODevice::ReadOnly | QIODevice::Text);
  return true;
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

  /* Tell the process to stop */
#if defined(Q_OS_WIN32)
  /* Tor on Windows doesn't understand a WM_CLOSE message (which is what 
   * QProcess::terminate() sends it), so we have to kill it harshly. */
  kill();
#else
  terminate();

  /* Wait for it to complete */
  if (!waitForFinished(-1)) {
    if (errmsg) {
      *errmsg = 
        QString("Process %1 failed to stop. [%2]").arg(pid()).arg(errorString());
    }
    return false;
  }
#endif
  return true;
}

/** Return the process ID for the current process. */
qint64
TorProcess::pid()
{
#if defined(Q_OS_WIN32)
  return (qint64)((QProcess::pid())->dwProcessId);
#else
  return QProcess::pid();
#endif
}

/** If the boolean value <b>log</b> is true, then the log() signal will be
 * emitted when Tor writes a log message to stdout. If <b>log</b> is false,
 * then no signal will be emitted for a log message. */
void
TorProcess::logStdout(bool log)
{
  if (!log) {
    /* We're going to stop monitoring log events on stdout, so make sure all
     * events up until now have been processed. */
    QCoreApplication::processEvents();
  }
  _log = log;
}

/** Called when there is data to be read from stdout */
void
TorProcess::onReadyRead()
{
  int i, j;
  while (canReadLine()) {
    /* Pull the waiting data from the buffer. Note that we do this even if the
     * log event won't be emitted, so that the size of the waiting buffer doesn't 
     * keep growing. */
    QString line = readLine();
    if (_log) {
      /* If _log is set, then we will parse the log message and emit log() */
      i = line.indexOf("[") ;
      j = line.indexOf("]") ;
      if (i > 0 && j > 0) {
        emit log(line.mid(i+1, j-i-1), line.mid(j+2));
      }
    }
  }
}

/** Returns a string description of the last QProcess::ProcessError
 * encountered. */
QString
TorProcess::errorString()
{
  QString err;
  switch (error()) {
    case QProcess::FailedToStart:
      err = "Failed to start"; break;
    case QProcess::Crashed:
      err = "Crashed"; break;
    case QProcess::Timedout:
      err = "Timed out"; break;
    case QProcess::WriteError:
      err = "Write error"; break;
    case QProcess::ReadError:
      err = "Read error"; break;
    default:
      err = "Unknown error"; break;
  }
  return err;
}

