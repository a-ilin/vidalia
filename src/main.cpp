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
 * \file main.cpp
 * \version $Id$
 * \brief Main Vidalia entry point
 */

#include <QObject>
#include <vidalia.h>
#include <gui/mainwindow.h>
#include <gui/common/vmessagebox.h>
#include <util/process.h>
#include <util/string.h>


/** Returns true if there is already another Vidalia process running. */
bool
is_vidalia_running(QString pidfile)
{
  /* Read the pidfile and find out if that process still exists */
  qint64 pid = read_pidfile(pidfile);
  if (pid > 0) {
    return (is_process_running(pid));
  }
  return false;
}

/** Main application entry point. */
int
main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(vidalia_common);
  QStringList args = char_array_to_stringlist(argv+1, argc-1);

  /* Construct the application object. Qt strips any command-line arguments
   * that it recognizes in argv, so we'll pass a stringlist of the original
   * list of command-line arguments too. */
  Vidalia vidalia(args, argc, argv);
  vNotice("Vidalia %1 using Qt %2").arg(Vidalia::version())
                                   .arg(QT_VERSION_STR);

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
  int ret = vidalia.exec();

  /* Vidalia exited, so cleanup our pidfile and return */
  QFile::remove(pidfile);
  vNotice("Vidalia is exiting cleanly (return code %1).").arg(ret);
  return ret;
}

