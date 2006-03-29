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

#include <vidalia.h>
#include <gui/mainwindow.h>


/** Main application entry point. */
int 
main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(vidalia);

  /* Construct the application object */
  Vidalia vidalia(argc, argv);

  /* Validate any command-line arguments */
  QString errmsg;
  if (!vidalia.validateArguments(errmsg)) {
    vidalia.printUsage(errmsg);
    return -1;
  }

  /* Since we don't have a visible main window, if we were to display a
   * QMessageBox (for example, to display an error when starting or stopping
   * Tor) then the application would exit when that message box was closed.
   * Setting quitOnLastWindowClosed to false fixes this behavior. */
  Vidalia::setQuitOnLastWindowClosed(false);
  
  /* Create an instance of the mainwindow and start the application */
  MainWindow mainWin; 
  return vidalia.exec();
}

