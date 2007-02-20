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
 * \brief Main Vidalia QApplication object
 */

#ifndef _VIDALIA_H
#define _VIDALIA_H

#if defined(Q_OS_WIN)
#include <windows.h>
#include <util/win32.h>
#endif

#include <QApplication>
#include <QMap>
#include <QString>

#include <util/log.h>
#include <gui/help/browser/helpbrowser.h>
#include <config/vidaliasettings.h>
#include <control/torcontrol.h>

/** Vidalia's version string */
#define VIDALIA_VERSION    "0.0.12-svn"

/** Pointer to this Vidalia application instance. */
#define vApp  ((Vidalia *)qApp)

#define vDebug(fmt)   (vApp->log(Log::Debug, (fmt)))
#define vInfo(fmt)    (vApp->log(Log::Info, (fmt)))
#define vNotice(fmt)  (vApp->log(Log::Notice, (fmt)))
#define vWarn(fmt)    (vApp->log(Log::Warn, (fmt)))
#define vError(fmt)   (vApp->log(Log::Error, (fmt)))


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
  static bool validateArguments(QString &errmsg);
  /** Displays usage information for command-line args. */
  static void showUsageMessageBox();
  /** Returns true if the user wants to see usage information. */
  static bool showUsage();
  
  /** Sets the current language. */
  static bool setLanguage(QString languageCode = QString());
  /** Sets the current GUI style. */
  static bool setStyle(QString styleKey = QString());
  
  /** Returns the current language. */
  static QString language() { return _language; }
  /** Returns the current GUI style. */
  static QString style() { return _style; }
  /** Returns Vidalia's application version. */
  static QString version() { return VIDALIA_VERSION; }

  /** Returns Vidalia's main TorControl object. */
  static TorControl* torControl() { return _torControl; }
  
  /** Returns the location Vidalia uses for its data files. */
  static QString dataDirectory();
  /** Returns the default location of Vidalia's data directory. */
  static QString defaultDataDirectory();
  
  /** Returns the location of Vidalia's pid file. */
  static QString pidFile();

  /** Writes <b>msg</b> with severity <b>level</b> to Vidalia's log. */
  static Log::LogMessage log(Log::LogLevel level, QString msg);

public slots:
  /** Shows the specified help topic, or the default if empty. */
  static void help(QString topic = QString());

signals:
  /** Signals that the application needs to shutdown now. */
  void shutdown();

protected:
#if defined(Q_OS_WIN)
  /** Filters Windows events, looking for events of interest */
  bool winEventFilter(MSG *msg, long *result);
#endif

private:
  /** Catches debugging messages from Qt and sends them to 
   * Vidalia's logs. */
  static void qt_msg_handler(QtMsgType type, const char *msg);

  /** Parse the list of command-line arguments. */
  void parseArguments(QStringList args);
  /** Returns true if the specified arguments wants a value. */
  bool argNeedsValue(QString argName);

  static QMap<QString, QString> _args; /**< List of command-line arguments.  */
  static QString _style;               /**< The current GUI style.           */
  static QString _language;            /**< The current language.            */

  static TorControl* _torControl;      /**< Vidalia's main TorControl object.*/
  static HelpBrowser* _help;           /**< Vidalia's configurable settings. */
  
  static Log _log; /**< Logs debugging messages to file or stdout. */
};

#endif

