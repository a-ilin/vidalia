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

/* Needed for _PROCESS_INFORMATION so that pid() works on Win32 */
#if defined (Q_OS_WIN32)
#include <windows.h>
#endif
  
#include "torprocess.h"

TorProcess::TorProcess()
{
}

TorProcess::~TorProcess()
{
}

/** Attempts to start the Tor process using the location, executable, and
 * command-line arguments specified in Vidalia's settings. If Tor doesn't
 * start, <b>errmsg</b> will be set appropriately and the function will return
 * false. */
bool
TorProcess::start(QFileInfo app, QStringList args, QString *errmsg)
{
  QString path = app.absoluteFilePath();
  QString exec;
  
  /* If the path doesn't point to an executable, then bail */
  if (!app.isExecutable()) {
    if (errmsg) {
      *errmsg = 
        QString("\"%1\" is not an executable.").arg(path);
    }
    return false;
  }
  
  /* If I simply pass the QStringList of command-line arguments to
   * QProcess::start(path, args), then Qt will quote any arguments with spaces
   * in them. This is logical, but Tor doesn't like it because you end up with
   * something like "-ControlPort 9051", which Tor thinks is a single
   * argument. So I'll just do the joining myself. */
  exec = path + " " + args.join(" ");
  
  /* Attempt to start Tor with the given command-line arguments */
  QProcess::start(exec);
  
  /* Wait for Tor to start. If it fails to start, report the error message and
   * return false. */
  if (!waitForStarted(-1)) {
    if (errmsg) {
      *errmsg = 
        QString("\"%1\" failed to start. [%2]").arg(path).arg(errorString());
    }
    return false;
  }
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
  terminate();

  /* Wait for it to complete */
  if (!waitForFinished(-1)) {
    if (errmsg) {
      *errmsg = 
        QString("Process %1 failed to stop. [%2]").arg(pid()).arg(errorString());
    }
    return false;
  }
  return true;
}

qint64
TorProcess::pid()
{
#if defined(Q_OS_WIN32)
  return (qint64)((QProcess::pid())->dwProcessId);
#else
  return QProcess::pid();
#endif
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

