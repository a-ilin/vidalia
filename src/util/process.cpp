/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
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
 * \file process.cpp
 * \version $Id$
 * \brief Process information and pidfile functions 
 */

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QApplication>

#include "stringutil.h"
#include "process.h"


/** Returns the PID of the current process. */
qint64
get_pid()
{
#if defined(Q_OS_WIN)
  return (qint64)GetCurrentProcessId();
#else
  return (qint64)getpid();
#endif
}

/** Returns true if a process with the given PID is running. */
bool
is_process_running(qint64 pid)
{
#if defined(Q_OS_WIN)
  QHash<qint64, QString> procList = win32_process_list();
  if (procList.contains(pid)) {
    /* A process with this ID exists. Check if it's the same as this process. */
    QString exeFile = procList.value(pid);
    QString thisExe = QFileInfo(QApplication::applicationFilePath()).fileName();
    return (exeFile.toLower() == thisExe.toLower());
  }
  return false;
#else
  /* Send the "null" signal to check if a process exists */
  if (kill((pid_t)pid, 0) < 0) {
    return (errno != ESRCH);
  }
  return true;
#endif
}

/** Writes the given file to disk containing the current process's PID. */
bool
write_pidfile(QString pidFileName, QString *errmsg)
{
  /* Make sure the directory exists */
  QDir pidFileDir = QFileInfo(pidFileName).absoluteDir();
  if (!pidFileDir.exists()) {
    pidFileDir.mkpath(QDir::convertSeparators(pidFileDir.absolutePath()));
  }

  /* Try to open (and create if it doesn't exist) the pidfile */
  QFile pidfile(pidFileName);
  if (!pidfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return err(errmsg, pidfile.errorString());
  }

  /* Write our current PID to it */
  QTextStream pidstream(&pidfile);
  pidstream << get_pid();
  return true;
}

/** Reads the given pidfile and returns the value contained in it. If the file
 * does not exist 0 is returned. Returns -1 if an error occurs. */
qint64 
read_pidfile(QString pidFileName, QString *errmsg)
{
  qint64 pid;
  
  /* Open the pidfile, if it exists */
  QFile pidfile(pidFileName);
  if (!pidfile.exists()) {
    return 0;
  }
  if (!pidfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (errmsg) {
      *errmsg = pidfile.errorString();
    }
    return -1; 
  }
  
  /* Read the PID in from the file */
  QTextStream pidstream(&pidfile);
  pidstream >> pid;
  return pid;
}

