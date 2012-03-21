/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/
/*
**  This file was originally written by Steven J. Murdoch, and
**  modified by Matt Edman. It is distributed under the following
**  license:
**
**  Copyright (C) 2007, Matt Edman
**  Copyright (C) 2007, Steven J. Murdoch
**                      <http://www.cl.cam.ac.uk/users/sjm217/>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin Street, Fifth Floor,
**  Boston, MA  02110-1301, USA.
*/

/*
** \file helperprocess.cpp
** \brief Invokes a web browser process (originally by Steven. J. Murdoch)
*/

#include "HelperProcess.h"
#include "Vidalia.h"

#include "stringutil.h"

#include <QString>
#include <QFileInfo>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

/** Default constructor */
HelperProcess::HelperProcess(QObject *parent)
: QProcess(parent)
{
  // Call error handling routine on errors
  QObject::connect(this, SIGNAL(error(QProcess::ProcessError)),
                   this, SLOT(onError(QProcess::ProcessError)));

  // Call output handling routines on process output
  QObject::connect(this, SIGNAL(readyReadStandardError()),
                   this, SLOT(onReadyReadStandardError()));
  QObject::connect(this, SIGNAL(readyReadStandardOutput()),
                   this, SLOT(onReadyReadStandardOutput()));
}

/** Invoked when output is written to the process's stderr. */
void
HelperProcess::onReadyReadStandardError()
{
  QString output = QString(readAllStandardError());
  foreach (QString line, output.split("\n")) {
    vInfo("(%1:stderr): %2").arg(_processName).arg(line);
  }
}

/** Invoked when output is written to the process's stdout. */
void
HelperProcess::onReadyReadStandardOutput()
{
  QString output = QString(readAllStandardOutput());
  foreach (QString line, output.split("\n")) {
    vInfo("(%1:stdout): %2").arg(_processName).arg(line);
  }
}

void
HelperProcess::start(const QString &app, const QString &args)
{
  QFileInfo fi(app);
  _processName = fi.fileName();

  QString commandLine = QString("\"%1\" %2").arg(app).arg(args);

   // Log the process name and arguments
  vNotice("Launching helper process with command line '%1'")
                                           .arg(commandLine);

  QProcess::start(commandLine, QIODevice::ReadOnly | QIODevice::Text);
}

/** Start the specified application. */
void
HelperProcess::start(const QString &app, const QStringList &args)
{
  // Remember the executable name of the process
  QFileInfo fi(app);
  _processName = fi.fileName();

  // Log the process name and arguments
  vNotice("Launching helper process '%1' with arguments '%2'").arg(app)
                                     .arg(string_format_arguments(args));

  // Start the specified application
  QProcess::start(app, args, QIODevice::ReadOnly | QIODevice::Text);
}

/** Invoked when underlying QProcess fails. */
void
HelperProcess::onError(QProcess::ProcessError error)
{
  // Pass up error messages on startup, but ignore the rest
  if (error == QProcess::FailedToStart) {
    vWarn("Helper process '%1' failed to start: %2").arg(_processName)
                                                    .arg(errorString());
    emit startFailed(errorString());
  }
}

/** Returns true iff process is not running. */
bool
HelperProcess::isDone() const
{
  return state() == NotRunning;
}

void
HelperProcess::toForeground()
{
#if defined(Q_WS_MAC)
  if(state() != QProcess::Running)
    return;

  ProcessSerialNumber psn;
  OSStatus st;

  do {
    st = GetProcessForPID(pid(), &psn);
    QCoreApplication::processEvents();
  } while(st == procNotFound and state() == QProcess::Running);

  if(st != procNotFound)
    SetFrontProcess(&psn);
#endif
}
