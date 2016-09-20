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
** \file vidalia.h
** \brief Main Vidalia QApplication object
*/

#ifndef _VIDALIA_H
#define _VIDALIA_H

#include "config.h"
#include "VidaliaSettings.h"
#include "TorSettings.h"
#include "Log.h"
#include "TorControl.h"
#include "Torrc.h"

#include <QApplication>
#include <QMap>
#include <QList>
#include <QString>
#include <QKeySequence>

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

  /** Validates that all arguments were well-formed. */
  bool validateArguments(QString &errmsg);
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

  /** Returns Vidalia's main TorControl object. */
  static Torrc* torrc() { return _torrc; }

  /** Returns the location Vidalia uses for its data files. */
  static QString dataDirectory();
  /** Returns the default location of Vidalia's data directory. */
  static QString defaultDataDirectory();

  /** Returns the location of Vidalia's pid file. */
  static QString pidFile();

  /** Returns true if Vidalia should read the control password from stdin.
   */
  static bool readPasswordFromStdin();

  /** Writes <b>msg</b> with severity <b>level</b> to Vidalia's log. */
  static Log::LogMessage log(Log::LogLevel level, QString msg);

  /** Enters the main event loop and waits until exit() is called. The signal
   * running() will be emitted when the event loop has started. */
  static int run();

  /** Creates and binds a shortcut such that when <b>key</b> is pressed in
   * <b>sender</b>'s context, <b>receiver</b>'s <b>slot</b> will be called. */
  static void createShortcut(const QKeySequence &key, QWidget *sender,
                             QObject *receiver, const char *slot);

  /** Creates and binds a shortcut such that when <b>key</b> is pressed in
   * <b>sender</b>'s context, <b>receiver</b>'s <b>slot</b> will be called. */
  static void createShortcut(const QString &key, QWidget *sender,
                             QObject *receiver, const char *slot);

  /** Loads and installs all available translators for the specified
   * <b>languageCode</b>. All currently installed QTranslator objects will be
   * removed. Returns true if at least Vidalia's language file can be loaded
   * for the given language. Otherwise, returns false and no change is made
   * to the current translators.
   */
  static bool retranslateUi(const QString &languageCode);

signals:
  /** Emitted when the application is running and the main event loop has
   * started. */
  void running();

protected:
#if defined(Q_OS_WIN)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  /** Filters Windows events, looking for events of interest */
  bool winEventFilter(MSG *msg, long *result);
#endif
#endif

  /** Removes all currently installed QTranslators. */
  static void removeAllTranslators();

private slots:
  /** Called when the application's main event loop has started. This method
   * will emit the running() signal to indicate that the application's event
   * loop is running. */
  void onEventLoopStarted();

private:
  /** Catches debugging messages from Qt and sends them to
   * Vidalia's logs. */
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  static void qt_msg_handler(QtMsgType type, const QMessageLogContext&, const QString& msg);
#else
  static void qt_msg_handler(QtMsgType type, const char *msg);
#endif

  /** Parse the list of command-line arguments. */
  void parseArguments(QStringList args);
  /** Returns true if the specified arguments wants a value. */
  bool argNeedsValue(QString argName);

  /** Copies a default settings file (if one exists) to Vidalia's data
   * directory.
   */
  void copyDefaultSettingsFile() const;

  static QMap<QString, QString> _args; /**< List of command-line arguments.  */
  static QString _style;               /**< The current GUI style.           */
  static QString _language;            /**< The current language.            */
  static TorControl* _torControl;      /**< Vidalia's main TorControl object.*/
  static Torrc* _torrc;      /**< Vidalia's main Torrc object.     */
  static Log _log; /**< Logs debugging messages to file or stdout. */
  static QList<QTranslator *> _translators; /**< List of installed translators. */

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  QAbstractNativeEventFilter* _nativeEventFilter;
#endif
};

#endif

