
#if defined(Q_OS_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <QFile>
#include <QTextStream>

#include "string.h"
#include "process.h"


/** Returns the PID of the current process. */
qint64
get_pid()
{
#if defined(Q_OS_WIN32)
  return (qint64)GetCurrentProcessId();
#else
  return (qint64)getpid();
#endif
}

/** Returns true if a process with the given PID is running. */
bool
is_process_running(qint64 pid)
{
#if defined(Q_OS_WIN32)
  HANDLE process = OpenProcess(0, FALSE, (DWORD)pid);
  if (process == NULL) {
    return false;
  }
  CloseHandle(process);
  return true;
#else
  return (getsid((pid_t)pid) != -1);
#endif
}

/** Writes the given file to disk containing the current process's PID. */
bool
write_pidfile(QString pidFileName, QString *errmsg)
{
  /* Try to open the pidfile */
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

