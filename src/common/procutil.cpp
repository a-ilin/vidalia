/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file procutil.cpp
** \version $Id$
** \brief Process information and pidfile functions 
*/

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QApplication>

#include "stringutil.h"
#include "procutil.h"


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

QHash<qint64, QString>
process_list()
{
#if defined(Q_OS_WIN32)
  return win32_process_list();
#else
  return QHash<qint64, QString>();
#endif
}

bool
process_kill(qint64 pid)
{
#if defined(Q_OS_WIN32)
  HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE,
                                static_cast<DWORD>(pid));
  if (hProcess == NULL)
    return false;

  BOOL ret = TerminateProcess(hProcess, 0);
  CloseHandle(hProcess);

  return (ret != FALSE);
#else
  return false;
#endif
}

