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
**  The append_string() function in this file is derived from the implementation
**  of strlcat() by Todd C. Miller. It is licensed as follows:
**
**  Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
**  All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions
**  are met:
**  1. Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**  3. The name of the author may not be used to endorse or promote products
**     derived from this software without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
**  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
**  THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
**  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
**  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
**  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
**  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
**  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
** \file CrashReporter.h
** \brief General routines to install a Breakpad-based exception handler and
** set options related to launching the crash reporting application.
*/

#include "CrashReporter.h"
#include "stringutil.h"

#if defined(Q_OS_WIN32)
#include <client/windows/handler/exception_handler.h>
#elif defined(Q_OS_MAC)
#include <client/mac/handler/exception_handler.h>
#include <sys/wait.h>
#include <fcntl.h>
#elif defined(Q_OS_LINUX)
#include <client/linux/handler/exception_handler.h>
#include <sys/wait.h>
#include <fcntl.h>
#elif defined(Q_OS_SOLARIS)
#include <client/solaris/handler/exception_handler.h>
#endif

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

#include <time.h>


namespace CrashReporter
{
#if defined(Q_OS_WIN32)
typedef wchar_t         _char_t;
typedef HANDLE          _file_handle_t;
#define PATH_SEPARATOR  TEXT("\\")
# ifdef _USE_32BIT_TIME_T
#   define TIME_TO_STRING(buf, buflen, t) \
      _ltoa_s(t, buf, buflen, 10)
# else
#   define TIME_TO_STRING(buf, buflen, t) \
      _i64toa_s(t, buf, buflen, 10)
# endif
#else
typedef char            _char_t;
typedef int             _file_handle_t;
#define PATH_SEPARATOR  "/"
#define TEXT(x)         (x)
#define TIME_TO_STRING(buf, buflen, t) \
  snprintf(buf, buflen, "%ld", t)
#endif

/** Pointer to the Breakpad-installed exception handler called if Vidalia
 * crashes.
 * \sa install_exception_handler()
 */
static google_breakpad::ExceptionHandler *exceptionHandler = 0;

/** If true, the crash reporting application will be displayed when the
 * Breakpad-installed exception handler is called. Otherwise, the system
 * will handle the exception itself.
 * \sa set_crash_reporter()
 */
static bool showCrashReporter = false;

/** Absolute path of the crash reporting application that will be launched
 * from the exception handler.
 * \sa set_crash_reporter()
 */
static _char_t crashReporterExecutable[MAX_PATH_LEN + 1] = TEXT("");

/** Version information for the application being monitored for crashes.
 * The version will be written to the extra information file alongside
 * the minidump.
 */
static char buildVersion[MAX_VERSION_LEN + 1] = "";

/** Path and filename of the application to restart after displaying
 * the crash reporting dialog. The contents of this string are encoded
 * in UTF-8.
 * \sa set_restart_options()
 */
static char restartExecutable[MAX_CMD_LEN + 1] = "";

/** Additional arguments to use when restarting the crashed application.
 * The contents of this string are encoded in UTF-8.
 * \sa set_restart_options()
 */
static char restartExecutableArgs[MAX_CMD_LEN + 1] = "";


/** Records the time at which install_exception_handler() is called, which
 * is usually as early as possible during application startup. This is used
 * in minidump_callback() to determine how long the application was running
 * before it crashed.
 * \sa install_exception_handler()
 * \sa minidump_callback()
 */
static time_t startupTime = 0;


/** Slightly modified version of the strlcat() implementation by Todd C.
 * Miller (see the top of this file or the LICENSE file for license details),
 * that supports arguments of either wchar_t* on Windows or the usual char*
 * everywhere else but retains the semantics of strlcat().
 */
static size_t
append_string(_char_t *dst, const _char_t *src, size_t siz)
{
  _char_t *d = dst;
  const _char_t *s = src;
  size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while (n-- != 0 && *d != TEXT('\0'))
    d++;
  dlen = d - dst;
  n = siz - dlen;

  if (n == 0)
#if defined(Q_OS_WIN32)
    return (dlen + wcslen(s));
#else
    return(dlen + strlen(s));
#endif

  while (*s != TEXT('\0')) {
    if (n != 1) {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = TEXT('\0');

  return(dlen + (s - src));  /* count does not include NUL */
}

/** Writes the formatted string "<b>key</b>=</b>val\n" to the file handle
 * specified by <b>hFile</b>. On Windows, <b>hFile</b> is a HANDLE. Everywhere
 * else, <b>hFile</b> is an int.
 */
static void
write_keyval_to_file(_file_handle_t hFile, const char *key, const char *val)
{
#if defined(Q_OS_WIN32)
  DWORD dwWritten;
  WriteFile(hFile, key, strlen(key), &dwWritten, NULL);
  WriteFile(hFile, "=", 1, &dwWritten, NULL);
  WriteFile(hFile, val, strlen(val), &dwWritten, NULL);
  WriteFile(hFile, "\n", 1, &dwWritten, NULL);
#else
  write(hFile, key, strlen(key));
  write(hFile, "=", 1);
  write(hFile, val, strlen(val));
  write(hFile, "\n", 1);
#endif
}

/** Writes to a file extra information used by the crash reporting
 * application such as how long the application was running before it
 * crashed, the application to restart, as well as any extra arguments.
 * The contents of the file are formatted as a series of "Key=Val\n" pairs.
 * The written file has the same path and base filename as the minidump
 * file, with ".info" appended to the end. Returns true if the file was
 * created succesfully. Otherwise, returns false.
 */
static bool
write_extra_dump_info(const _char_t *path, const _char_t *id, time_t crashTime)
{
  static const char *KeyBuildVersion = "BuildVersion";
  static const char *KeyCrashTime = "CrashTime";
  static const char *KeyStartupTime = "StartupTime";
  static const char *KeyRestartExecutable = "RestartExecutable";
  static const char *KeyRestartExecutableArgs = "RestartExecutableArgs";

  _char_t extraInfoPath[MAX_PATH_LEN] = TEXT("");
  append_string(extraInfoPath, path, MAX_PATH_LEN);
  append_string(extraInfoPath, PATH_SEPARATOR, MAX_PATH_LEN);
  append_string(extraInfoPath, id, MAX_PATH_LEN);
  size_t len = append_string(extraInfoPath, TEXT(".dmp.info"), MAX_PATH_LEN);
  if (len >= MAX_PATH_LEN)
    return false;

#if defined(Q_OS_WIN32)
  HANDLE hFile = CreateFile(extraInfoPath, GENERIC_WRITE, 0, NULL,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;
#else
  _file_handle_t hFile = creat(extraInfoPath, S_IRUSR | S_IWUSR);
  if(hFile == -1) {
    return false;
  }
#endif

  char crashTimeString[24], startupTimeString[24];
  TIME_TO_STRING(crashTimeString, 24, crashTime);
  TIME_TO_STRING(startupTimeString, 24, startupTime);

  write_keyval_to_file(hFile, KeyBuildVersion, buildVersion);
  write_keyval_to_file(hFile, KeyCrashTime, crashTimeString);
  write_keyval_to_file(hFile, KeyStartupTime, startupTimeString);
  write_keyval_to_file(hFile, KeyRestartExecutable, restartExecutable);
  write_keyval_to_file(hFile, KeyRestartExecutableArgs, restartExecutableArgs);

#if defined(Q_OS_WIN32)
  CloseHandle(hFile);
#else
  close(hFile);
#endif
  return true;
}

/** Breakpad-installed exception handler. This function gets called in the
 * event of a crash. If <b>showCrashReporter</b> is true, this will execute
 * the crash reporting application, passing it the name and location of the
 * generated minidump, the absolute path to the (now crashed) Vidalia
 * executable, and any arguments that may be needed to restart Vidalia.
 */
bool
minidump_callback(const _char_t *path,    // Path to the minidump file
                  const _char_t *id,      // Minidump UUID
                  void *context,          // Callback context
#if defined(Q_OS_WIN32)
                  EXCEPTION_POINTERS *exInfo,
                  MDRawAssertionInfo *assertionInfo,
#endif
                  bool succeeded)
{
  if (! succeeded || ! showCrashReporter)
    return false;

  /* Write the extra dump info, such as application uptime, executable to
   * restart, and any necessary restart arguments. */
  write_extra_dump_info(path, id, time(NULL));

#if defined(Q_OS_WIN32)
  /* Format the command line used to launch the crash reporter */
  _char_t commandLine[MAX_CMD_LEN] = TEXT("");
  append_string(commandLine, TEXT("\""), MAX_CMD_LEN);
  append_string(commandLine, crashReporterExecutable, MAX_CMD_LEN);
  append_string(commandLine, TEXT("\" \""), MAX_CMD_LEN);
  append_string(commandLine, path, MAX_CMD_LEN);
  append_string(commandLine, PATH_SEPARATOR, MAX_CMD_LEN);
  append_string(commandLine, id, MAX_CMD_LEN);
  size_t len = append_string(commandLine, TEXT(".dmp\""), MAX_CMD_LEN);
  if (len >= MAX_CMD_LEN)
    return false;

  /* Launch the crash reporter with the name and location of the minidump */
  PROCESS_INFORMATION pi;
  STARTUPINFOW si;

  ZeroMemory(&pi, sizeof(pi));
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOWDEFAULT;

  BOOL rc = CreateProcess(NULL, (LPWSTR)commandLine, NULL, NULL, FALSE, 0,
                          NULL, NULL, &si, &pi);
  if (rc) {
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }
  TerminateProcess(GetCurrentProcess(), 1);
  return true;
#else
  /* Format the command line used to launch the crash reporter */
  _char_t args[MAX_CMD_LEN] = TEXT("");
  size_t len;

  append_string(args, path, MAX_CMD_LEN);
  append_string(args, PATH_SEPARATOR, MAX_CMD_LEN);
  append_string(args, id, MAX_CMD_LEN);
  len = append_string(args, TEXT(".dmp"), MAX_CMD_LEN);
  if (len >= MAX_CMD_LEN)
    return false;

  char *nargs[] = {crashReporterExecutable, args, NULL};

  pid_t p = fork(), ret;
  int status;
  if(p == 0) {
    execv(crashReporterExecutable, nargs);
  } else {
    ret = wait(&status);
    if(ret == -1)
      return false;
  }
  return true;
#endif
}

bool
install_exception_handler(const QString &dumpPath)
{
  /* Create a directory for the crash dumps if it doesn't already exist */
  QDir dumpDir(dumpPath);
  if (! dumpDir.exists() && ! dumpDir.mkdir("."))
    return false;

  /* Create the exception handler and specify where the dumps should go */
  exceptionHandler = new google_breakpad::ExceptionHandler(
#if defined(Q_OS_WIN32)
                            dumpDir.absolutePath().toStdWString(),
#else
                            dumpDir.absolutePath().toStdString(),
#endif
                            NULL,
                            minidump_callback,
                            NULL,
#if defined(Q_OS_WIN32)
                            google_breakpad::ExceptionHandler::HANDLER_ALL);
#else
                            true);
#endif
  if (! exceptionHandler)
    return false;

  startupTime = time(NULL);
  return true;
}

void
remove_exception_handler(void)
{
  if (exceptionHandler) {
    delete exceptionHandler;
    exceptionHandler = 0;
  }
}

bool
set_crash_reporter(const QString &crashReporter)
{
#if defined(Q_OS_WIN32)
  if (crashReporter.length() <= CrashReporter::MAX_PATH_LEN) {
    crashReporter.toWCharArray(crashReporterExecutable);
    crashReporterExecutable[crashReporter.length()] = L'\0';
#else
  QByteArray utf8 = crashReporter.toUtf8();
  if (utf8.length() <= CrashReporter::MAX_PATH_LEN) {
    memcpy(crashReporterExecutable, utf8.constData(), utf8.length());
    crashReporterExecutable[utf8.length()] = '\0';
#endif
    showCrashReporter = true;
  } else {
    /* If the given path is longer than MAX_PATH_LEN, no crash reporting
     * application will be set since the user's platform wouldn't be able to
     * execute it anyway.
     */
    showCrashReporter = false;
  }
  return showCrashReporter;
}

bool
set_restart_options(const QString &executable, const QStringList &arguments)
{
  QByteArray exe = executable.toUtf8();
  if (exe.length() > MAX_CMD_LEN)
    return false;

  QByteArray args = string_format_arguments(arguments).toUtf8();
  if (args.length() > MAX_CMD_LEN)
    return false;

  memcpy(restartExecutable, exe.constData(), exe.length());
  restartExecutable[exe.length()] = '\0';

  memcpy(restartExecutableArgs, args.constData(), args.length());
  restartExecutableArgs[args.length()] = '\0';

  return true;
}

bool
set_build_version(const QString &version)
{
  if (version.length() > MAX_VERSION_LEN)
    return false;

  QByteArray ascii = version.toAscii();
  memcpy(buildVersion, ascii.constData(), ascii.length());
  buildVersion[ascii.length()] = '\0';

  return true;
}

}

