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
** \file CrashReporter.h
** \brief General routines to install a Breakpad-based exception handler and
** set options related to launching the crash reporting application.
*/

#ifndef _CRASHREPORTER_H
#define _CRASHREPORTER_H

class QString;
class QStringList;


namespace CrashReporter
{
  /** Defines the maximum length of the absolute path plus filename of the
   * crash reporting executable displayed when the exception handler is
   * called.
   */
#if defined(Q_OS_WIN32)
  static const int MAX_PATH_LEN = 260;
#else
  static const int MAX_PATH_LEN = 4096; /* Is there a better value for this? */
#endif

  /** Defines the maximum length of the command line arguments used to restart
   * the crashed application by the crash reporter. The maximum command line
   * length is based on Windows' 32K character command line limit, according
   * to the MSDN documents.
   */
  static const int MAX_CMD_LEN = 32768;

  /** Defines the maximum length of a build version string that can be set
   * by set_build_version().
   * \sa set_build_version()
   */
  static const int MAX_VERSION_LEN = 64;

  /** Installs the Breakpad exception handler and sets the static global
   * variables used by the exception handler to launch the crash reporting
   * application. Minidumps will be writen to <b>dumpPath</b>, which will
   * be created if it doesn't already exist.
   * \sa remove_exception_handler()
   */
  bool install_exception_handler(const QString &dumpPath);

  /** Removes the application's exception handler previously created by
   * install_exception_handler(). If no exception handler was previously created,
   * no action will be taken.
   * \sa install_exception_handler()
   */
  void remove_exception_handler(void);

  /** Sets <b>crashReporter</b> as the executable that gets called when the
   * exception handler catches a crash. If <b>crashReporter</b> contains one
   * or more spaces, the given path will be wrapped in quotes. The caller is
   * responsible for ensuring that <b>crashReporter</b> is no greater than
   * CrashReporter::MAX_PATH_LEN (including added quotes). Returns true if
   * the crash reporting application was set successfully, or false if
   * <b>crashReporter</b> was too long.
   */
  bool set_crash_reporter(const QString &crashReporter);

  /** Sets the <b>executable</b> and <b>args</b> that will be passed to the
   * crash reporting application, so it can restart the crashed application
   * with the same arguments as before it crashed. If the <b>executable</b>
   * path or any of <b>args</b> contains a space, they will be quoted before
   * being passed to the crash reporting application. The path to the
   * generated minidump, crash reporting application, executable to restart
   * and any arguments must fit within MAX_CMD_LEN, including any added
   * quotes.
   * \sa set_crash_reporter()
   */
  bool set_restart_options(const QString &executable,
                           const QStringList &arguments);

  /** Sets <b>version</b> as the build version identifier written to the
   * extra information file alongside a minidump. The version string must
   * be no longer than CrashReporter::MAX_VERSION_LEN.
   */
  bool set_build_version(const QString &version);
}

#endif

