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
#include <vidalia.h>
#include <gui/mainwindow.h>
#include <util/string.h>

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


#if !defined(Q_OS_WIN32)
  /* Validate any command-line arguments. Don't bother doing this on Win32
   * since they can't see the output anyway. */
  QString errmsg;
  if (!vidalia.validateArguments(errmsg)) {
    vidalia.printUsage(errmsg);
    return -1;
  }
#endif

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
  return vidalia.exec();
}

