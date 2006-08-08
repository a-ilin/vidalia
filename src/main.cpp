/****************************************************************
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

/**
 * \file main.cpp
 * \version $Id$
 *
 * Contains the main Vidalia entry point.
 */

#include <QObject>
#include <QPictureFormatPlugin>
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
  Q_IMPORT_PLUGIN(QJpegPlugin);
  QStringList args = char_array_to_stringlist(argv+1, argc-1);

  /* Construct the application object. Qt strips any command-line arguments
   * that it recognizes in argv, so we'll pass a stringlist of the original
   * list of command-line arguments too. */
  Vidalia vidalia(args, argc, argv);


#if !defined(Q_OS_WIN32)
  /* Validate any command-line arguments. Don't bother doing this on Win32
   * since they can't see the output anyway. */
  QString errmsg;
  if (!vidalia.validateArguments(errmsg)) {
    vidalia.printUsage(errmsg);
    return -1;
  }
#endif

  /* Check if Vidalia is already running. */
  QString pidfile = vidalia.pidFile();
  if (is_vidalia_running(pidfile)) {
    /* Let the user know another Vidalia is running and we are going to exit
     * now. */
    VMessageBox::critical(0, 
      qApp->translate("Vidalia",
        QT_TRANSLATE_NOOP("Vidalia", "Vidalia is already running")),
      qApp->translate("Vidalia",
        QT_TRANSLATE_NOOP("Vidalia", 
          "Another Vidalia process is already running. "
          "This Vidalia process will now exit.")),
      VMessageBox::Ok);   
    return 0;
  }
  write_pidfile(pidfile);

  /* Since we don't have a visible main window, if we were to display a
   * QMessageBox (for example, to display an error when starting or stopping
   * Tor) then the application would exit when that message box was closed.
   * Setting quitOnLastWindowClosed to false fixes this behavior. */
  Vidalia::setQuitOnLastWindowClosed(false);

  /* Create an instance of the mainwindow and start the application */
  MainWindow mainWin;

  /* Handle the shutdown signal by closing the mainwindow, which closes
   * all the necessary child windows. */
  QObject::connect(&vidalia, SIGNAL(shutdown()), &mainWin, SLOT(close()));

  /* Run Vidalia */
  int ret = vidalia.exec();

  /* Vidalia exited, so cleanup our pidfile and return */
  QFile::remove(vidalia.pidFile());
  return ret;
}

