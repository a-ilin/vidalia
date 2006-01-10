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
  
  /* If the path doesn't point to an executable, then bail */
  if (!app.isExecutable()) {
    if (errmsg) {
      *errmsg = 
        QString("\"%1\" is not an executable.").arg(path);
    }
    return false;
  }
  
  /* Attempt to start Tor */
  QProcess::start(path, args);
  
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
  /* Tell the process to stop */
  kill();

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

