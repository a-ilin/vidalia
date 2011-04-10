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
** \file main.cpp
** \brief Main Vidalia entry point
*/

#include "config.h"
#include "Vidalia.h"
#include "MainWindow.h"
#include "VMessageBox.h"
#if defined(USE_BREAKPAD)
#include "CrashReporter.h"
#endif

#include "procutil.h"
#include "stringutil.h"

#include <QObject>
#if defined(Q_OS_WIN32)
#include <QSysInfo>
#endif
#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif

#if defined(USE_BREAKPAD)
void
setup_crash_reporter()
{
  /* Set the crash reporting application used to submit minidumps. On Windows,
   * crashreporter.exe is assumed to live in the same directory as vidalia.exe.
   */
#if defined(Q_OS_WIN32)
  QString crashReporter = Vidalia::applicationDirPath() + "\\crashreporter.exe";
#elif defined(Q_OS_MAC)
  QString crashReporter = Vidalia::applicationDirPath() + "/CrashReporter.app";
#endif
  if (! QFileInfo(crashReporter).isExecutable()) {
    vWarn("Unable to find crash reporting application. Crash reporting will "
          "be unavailable.");
    return;
  }
  if (! CrashReporter::set_crash_reporter(crashReporter)) {
    vWarn("Vidalia found the crash reporting application, but the path is "
          "longer than your platform supports. Skipping crash reporting.");
    return;
  }

  /* Set the Vidalia executable and options used to restart Vidalia after a 
   * crash. We strip off the first argument in Vidalia::arguments(), since
   * it's the application name again anyway. */
  CrashReporter::set_restart_options(Vidalia::applicationFilePath(),
                                     Vidalia::arguments().mid(1));

  /* Set the build version that gets saved with a minidump. */
  CrashReporter::set_build_version(VIDALIA_VERSION);

  /* Install the exception handler and give it the location to which Breakpad
   * should write its minidumps. */
  QString dumpPath = Vidalia::dataDirectory() + "/crashreports";
  if (! CrashReporter::install_exception_handler(dumpPath)) {
    vWarn("Unable to setup Breakpad exception handler. Crash reporting "
          "will be unavailable.");
  } else {
    vInfo("Installed Breakpad exception handler.");
  }
}
#endif

extern "C" void
signal_handler(int signal)
{
#ifdef HAVE_SIGNAL_H
  if (signal == SIGINT || signal == SIGTERM)
    vApp->quit();
#endif
}

void
install_signal_handler()
{
#if defined(HAVE_SIGACTION)
  struct sigaction action;

  sigemptyset(&action.sa_mask);
  action.sa_handler = signal_handler;
  action.sa_flags   = 0;

  if (sigaction(SIGINT,  &action, NULL) < 0)
    vWarn("Failed to install SIGINT handler.");
  if (sigaction(SIGTERM, &action, NULL) < 0)
    vWarn("Failed to install SIGTERM handler.");
#elif defined(HAVE_SIGNAL)
  if (signal(SIGINT, signal_handler) == SIG_ERR)
    vWarn("Failed to install SIGINT handler.");
  if (signal(SIGTERM, signal_handler) == SIG_ERR)
    vWarn("Failed to install SIGTERM handler.");
#endif
}

/** Returns true if there is already another Vidalia process running. */
bool
is_vidalia_running(const QString &pidfile)
{
  /* Read the pidfile and find out if that process still exists */
  qint64 pid = read_pidfile(pidfile);
  if (pid > 0) {
#if defined(Q_OS_WIN32)
    if (QSysInfo::WindowsVersion == QSysInfo::WV_NT) {
      /* We currently can't get a list of running processes on Windows NT, so
       * be pessimistic and assume the existence of a nonzero pidfile means
       * Vidalia is running. */
      return true;
    } else
      return (is_process_running(pid));
#else
    return (is_process_running(pid));
#endif
  }
  return false;
}

/** Main application entry point. */
int
main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(vidalia);
  QStringList args = char_array_to_stringlist(argv+1, argc-1);
 
  /* Construct the application object. Qt strips any command-line arguments
   * that it recognizes in argv, so we'll pass a stringlist of the original
   * list of command-line arguments too. */
  Vidalia vidalia(args, argc, argv);
  vNotice("Vidalia %1 using Qt %2").arg(Vidalia::version())
                                   .arg(QT_VERSION_STR);

#if defined(USE_BREAKPAD)
  /* Set up the crash reporting application and exception handlers. */
  setup_crash_reporter();
#endif
#if defined(USE_MARBLE) && defined(Q_OS_WIN32)
  vApp->addLibraryPath(vApp->applicationDirPath() + "/plugins/qt");
#endif

  /* Install a signal handler to clean up properly after a catching a 
   * SIGINT or SIGTERM. */
  install_signal_handler();

  /* Validate any command-line arguments, or show usage message box, if
   * necessary. */
  QString errmsg;
  if (vidalia.showUsage()) {
    Vidalia::showUsageMessageBox();
    return 0;
  } else if (!vidalia.validateArguments(errmsg)) {
    vError("Unable to apply command-line arguments: %1").arg(errmsg);
    VMessageBox::critical(0,
      vApp->translate("Vidalia",
        QT_TRANSLATE_NOOP("Vidalia", "Invalid Argument")), errmsg,
      VMessageBox::Ok);
    return 1;
  }

  /* Check if Vidalia is already running. */
  QString pidfile = vidalia.pidFile();
  if (is_vidalia_running(pidfile)) {
    vWarn("Detected another process with pid %1. Is Vidalia already running?")
                                                               .arg(get_pid());
    /* Let the user know another Vidalia is running and we are going to exit
     * now. */
    int ret = VMessageBox::critical(0, 
                vApp->translate("Vidalia",
                  QT_TRANSLATE_NOOP("Vidalia", "Vidalia is already running")),
                vApp->translate("Vidalia",
                  QT_TRANSLATE_NOOP("Vidalia", 
                    "Another Vidalia process is possibly already running. "
                    "If there really is not another Vidalia process running, "
                    "you can choose to continue anyway.\n\n"
                    "Would you like to continue starting Vidalia?")),
                VMessageBox::Continue, VMessageBox::Quit|VMessageBox::Default);
    if (ret != VMessageBox::Continue) {
      /* Don't start a second instance of Vidalia */
      vError("Exiting duplicate Vidalia process.");
      return 1;
    }
  }
  write_pidfile(pidfile);

  /* Since we don't have a visible main window, if we were to display a
   * QMessageBox (for example, to display an error when starting or stopping
   * Tor) then the application would exit when that message box was closed.
   * Setting quitOnLastWindowClosed to false fixes this behavior. */
  Vidalia::setQuitOnLastWindowClosed(false);

  /* Create an instance of the main window  */
  MainWindow mainWin;

  /* Run Vidalia */
  int ret = vidalia.run();

  /* Vidalia exited, so cleanup our pidfile and return */
  QFile::remove(pidfile);
  vNotice("Vidalia is exiting cleanly (return code %1).").arg(ret);

#if defined(USE_BREAKPAD)
  vInfo("Removing Breakpad exception handler.");
  CrashReporter::remove_exception_handler();
#endif

  return ret;
}

