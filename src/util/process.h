
#ifndef _PROCESS_H
#define _PROCESS_H

#include <QString>


/** Returns the PID of the current process. */
qint64 get_pid();

/** Returns true if a process with the given PID is running. */
bool is_process_running(qint64 pid);

/** Writes the given file to disk containing the current process's PID. */
bool write_pidfile(QString pidfile, QString *errmsg = 0);

/** Reads the giiven pidfile and returns the value in it. If the file does not
 * exist, -1 is returned. */
qint64 read_pidfile(QString pidfile, QString *errmsg = 0);

#endif

