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
 * \file vidalia.h
 * \version $Id$
 */

#ifndef _VIDALIA_H
#define _VIDALIA_H

#include <QApplication>
#include <QMap>
#include <QString>

#include <gui/help/browser/helpbrowser.h>
#include <config/vidaliasettings.h>
#include <control/torcontrol.h>

#define VIDALIA_VERSION   "0.0.2-svn"


class Vidalia : public QApplication
{
  Q_OBJECT

public:
  /** Constructor. */
  Vidalia(QStringList args, int &argc, char **argv);
  /** Destructor. */
  ~Vidalia();
  
  /** Return the map of command-line arguments and values. */
  static QMap<QString, QString> arguments() { return _args; }
  /** Validates that all arguments were well-formed. */
  bool validateArguments(QString &errmsg = QString());
  /** Prints usage information to the given text stream. */
  void printUsage(QString errmsg = QString());
  
  /** Sets the current language. */
  static bool setLanguage(QString languageCode = QString()); 
  /** Sets the current GUI style. */
  static bool setStyle(QString styleKey = QString());
  /** Shows the specified help topic, or the default if empty. */
  static void help(QString topic = QString());
  
  /** Returns the current language. */
  static QString language() { return _language; }
  /** Returns the current GUI style. */
  static QString style() { return _style; }
  /** Returns Vidalia's application version. */
  static QString version() { return VIDALIA_VERSION; }

  /** Returns Vidalia's main TorControl object. */
  static TorControl* torControl() { return _torControl; }

private:
  /** Parse the list of command-line arguments. */
  void parseArguments(QStringList args);

  static QMap<QString, QString> _args; /**< List of command-line arguments.  */
  static QString _style;               /**< The current GUI style.           */
  static QString _language;            /**< The current language.            */
  static VidaliaSettings _settings;    /**< Vidalia's configurable settings. */
  
  static TorControl* _torControl;      /**< Vidalia's main TorControl object.*/
  static HelpBrowser* _help;           /**< Vidalia's configurable settings. */
};

#endif

