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
 * On Mac, it will use the native Mac style and use the Plastique style on all
 * other platforms.
 */
 
#include <QApplication>
#include <QTextStream>
#include <QStyleFactory>

#include <lang/languagesupport.h>
#include <config/vidaliasettings.h>
#include <gui/mainwindow.h>


/** Prints the available arguments for Vidalia. The format of the usage
 * message is:
 *
 * Usage:
 *   vidalia [options]
 *
 * Available Options:
 *   -help          Displays this usage message and exits.
 *   -reset         Resets ALL stored Vidalia settings.
 */
void
print_usage()
{
  QTextStream out(stdout);
  out << "Usage: " << endl;
  out << "\t" << QCoreApplication::arguments().at(0) << " [options]" << endl;
  out << endl << "Available Options:" << endl;
  out << "\t-help\t\tDisplays this usage message and exits." << endl;
  out << "\t-reset\t\tResets ALL stored Vidalia settings." << endl;
  out << endl;
}

/** A simple wrapper that handles stripping a leading "-" or "--", if given,
 * on a command-line argument and converting the argument to all-lowercase. */
QString
get_argument_name(QString arg)
{
  if (arg.startsWith("-")) {
    arg = arg.mid((arg.startsWith("--") ? 2 : 1));
  }
  return arg.toLower();
}

/** Returns true if the specified argument requires a value. */
bool
argument_requires_value(QString arg)
{
  Q_UNUSED(arg);
  return false;
}

/** Parses the arguments specified on the command-line and handles arguments
 * that are followed by values. */
bool
parse_arguments(QMap<QString, QString> &argMap)
{
  QStringList argv = QCoreApplication::arguments();
  
  QString arg, value;
  for (int i = 1; i < argv.size(); i++) {
    arg   = get_argument_name(argv.at(i));
    value = "";
    
    /* Handle arguments with values */ 
    if (argument_requires_value(arg)) {
      if (i >= argv.size() - 1) {
        return false;
      }
      value = argv.at(++i);
    }
    argMap.insert(arg, value);
  }
  return true;
}

/** Main application entry point. */
int 
main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(vidalia);
  VidaliaSettings settings;

  /* Parse and validate any command-line arguments */
  QApplication app(argc, argv);
  QMap<QString,QString> argMap;
  if (!parse_arguments(argMap) || argMap.contains("help")) {
    print_usage();
    return -1;
  }
   
  /* Check if we're supposed to reset our saved configuration */
  if (argMap.contains("reset")) {
    VidaliaSettings::reset();
  }
  
  /* Translate the application's strings */
  LanguageSupport::translate(settings.getLanguageCode());

  /* Apply the saved gui style */
  QApplication::setStyle(QStyleFactory::create(settings.getInterfaceStyle()));
  
  /* Since we don't have a visible main window, if we were to display a
   * QMessageBox (for example, to display an error when starting or stopping
   * Tor) then the application would exit when that message box was closed.
   * Setting quitOnLastWindowClosed to false fixes this behavior. */
  QApplication::setQuitOnLastWindowClosed(false);
  
  /* Create an instance of the mainwindow and start the application */
  MainWindow mainWin; 
  return app.exec();
}

