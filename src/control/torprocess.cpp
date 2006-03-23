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
 * \file torprocess.cpp
 * \version $Id$
 */

#include <QFileInfo>
#include <QCoreApplication>

/* Needed for _PROCESS_INFORMATION so that pid() works on Win32 */
#if defined (Q_OS_WIN32)
#include <windows.h>
#endif
  
#include "torprocess.h"

/** Format of log message timestamps Tor prints to stdout */
#define FMT_TIMESTAMP "MMM dd hh:mm:ss.zzz yyyy"

/** Default constructor */
TorProcess::TorProcess()
{
  openStdout();
  connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyRead()));
}

/** Attempts to start the Tor process using the location, executable, and
 * command-line arguments specified in Vidalia's settings. If Tor doesn't
 * exist at the given path, <b>errmsg</b> will be set appropriately and the
 * function will return false. */
bool TorProcess::start(QString app, QString args, QString *errmsg) 
{
  QFileInfo appInfo(app);
  QString path = appInfo.absoluteFilePath();
  
  /* If the path doesn't point to an executable, then bail */
  if (!appInfo.isExecutable()) {
    if (errmsg) {
      *errmsg = 
        tr("\"%1\" is not an executable.").arg(path);
    }
    return false;
  }
  
  /* This lovely little hack here is because the isExecutable() method above
   * doesn't understand quoted paths, but the QProcess::start() method below
   * requires it, on Windows. */
#if defined(Q_OS_WIN32)
  path = "\"" + path + "\"";
#endif

  /* Attempt to start Tor with the given command-line arguments */
  QProcess::start(path + " " + args, QIODevice::ReadOnly | QIODevice::Text);
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
  _logState = Open;
  setReadChannelMode(QProcess::MergedChannels);
  setReadChannel(QProcess::StandardOutput);
}

/** Closes logging on stdout. When this is closed, the log() signal will not
 * be emitted when Tor prints a message to stdout. */
void
TorProcess::closeStdout()
{
  _logState = Closing;
  _logCloseTime = QDateTime::currentDateTime();
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
    if (_logState == Closed) {
      continue;
    }
    
    /* If _logState != Closed, then we will parse the log message and emit log() */
    i = line.indexOf("[");
    j = line.indexOf("]");
    if (i > 0 && j > 0) {
      if (_logState == Closing) {
        /* Parse the timestamp from this log message */
        QString msgTime = QString("%1 %2").arg(line.mid(0, i-1))
                                          .arg(_logCloseTime.date().year());
          
        if (_logCloseTime < QDateTime::fromString(msgTime, FMT_TIMESTAMP)) {
          /* We've read all log messages from stdout since closeStdout() was
           * called, so go ahead and close stdout for real. */
          _logState = Closed;
          closeReadChannel(QProcess::StandardOutput);
        }
      }
      if (_logState != Closed) {
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
      err = tr("Failed to start"); break;
    case QProcess::Crashed:
      err = tr("Crashed"); break;
    case QProcess::Timedout:
      err = tr("Timed out"); break;
    case QProcess::WriteError:
      err = tr("Write error"); break;
    case QProcess::ReadError:
      err = tr("Read error"); break;
    default:
      err = tr("Unknown error"); break;
  }
  return err;
}

