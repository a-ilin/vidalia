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
#include <QKeySequence>

#include <util/log.h>
#include <helpbrowser.h>
#include <vidaliasettings.h>
#include <torcontrol.h>

/** Vidalia's version string */
#define VIDALIA_VERSION    "0.0.15"

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
 
  /** Enters the main event loop and waits until exit() is called. The signal
   * running() will be emitted when the event loop has started. */
  static int run();

  /** Creates and binds a shortcut such that when <b>key</b> is pressed in
   * <b>sender</b>'s context, <b>receiver</b>'s <b>slot</b> will be called. */
  static void createShortcut(const QKeySequence &key, QWidget *sender,
                             QWidget *receiver, const char *slot);

public slots:
  /** Shows the specified help topic, or the default if empty. */
  static void help(QString topic = QString());

signals:
  /** Emitted when the application is running and the main event loop has
   * started. */ 
  void running();
  /** Signals that the application needs to shutdown now. */
  void shutdown();

protected:
#if defined(Q_OS_WIN)
  /** Filters Windows events, looking for events of interest */
  bool winEventFilter(MSG *msg, long *result);
#endif

private slots:
  /** Called when the application's main event loop has started. This method
   * will emit the running() signal to indicate that the application's event
   * loop is running. */
  void onEventLoopStarted();
  
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

