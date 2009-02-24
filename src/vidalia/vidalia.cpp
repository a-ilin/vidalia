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
** \file vidalia.cpp
** \version $Id$
** \brief Main Vidalia QApplication object
*/

#include <QDir>
#include <QTimer>
#include <QTextStream>
#include <QStyleFactory>
#include <QShortcut>
#include <QTranslator>
#include <QLibraryInfo>
#include <languagesupport.h>
#include <vmessagebox.h>
#include <stringutil.h>
#include <html.h>
#include <stdlib.h>
#include "config.h"
#include "vidalia.h"

#ifdef Q_OS_MACX
#include <Carbon/Carbon.h>
#endif
#ifdef USE_MARBLE
#include <MarbleDirs.h>
#endif

/* Available command-line arguments. */
#define ARG_LANGUAGE   "lang"     /**< Argument specifying language.    */
#define ARG_GUISTYLE   "style"    /**< Argument specfying GUI style.    */
#define ARG_RESET      "reset"    /**< Reset Vidalia's saved settings.  */
#define ARG_HELP       "help"     /**< Display usage informatino.       */
#define ARG_DATADIR    "datadir"  /**< Directory to use for data files. */
#define ARG_PIDFILE    "pidfile"  /**< Location and name of our pidfile.*/
#define ARG_LOGFILE    "logfile"  /**< Location of our logfile.         */
#define ARG_LOGLEVEL   "loglevel" /**< Log verbosity.                   */
#define ARG_READ_PASSWORD_FROM_STDIN  \
  "read-password-from-stdin" /**< Read password from stdin. */

/* Static member variables */
QMap<QString, QString> Vidalia::_args; /**< List of command-line arguments.  */
QString Vidalia::_style;               /**< The current GUI style.           */
QString Vidalia::_language;            /**< The current language.            */
TorControl* Vidalia::_torControl = 0;  /**< Main TorControl object.          */
Log Vidalia::_log;
QList<QTranslator *> Vidalia::_translators;

/** Catches debugging messages from Qt and sends them to Vidalia's logs. If Qt
 * emits a QtFatalMsg, we will write the message to the log and then abort().
 */
void
Vidalia::qt_msg_handler(QtMsgType type, const char *s)
{
  QString msg(s);
  switch (type) {
    case QtDebugMsg:
      vDebug("QtDebugMsg: %1").arg(msg);
      break;
    case QtWarningMsg:
      vNotice("QtWarningMsg: %1").arg(msg);
      break;
    case QtCriticalMsg:
      vWarn("QtCriticalMsg: %1").arg(msg);
      break;
    case QtFatalMsg:
      vError("QtFatalMsg: %1").arg(msg);
      break;
  }
  if (type == QtFatalMsg) {
    vError("Fatal Qt error. Aborting.");
    abort();
  }
}

/** Constructor. Parses the command-line arguments, resets Vidalia's
 * configuration (if requested), and sets up the GUI style and language
 * translation. */
Vidalia::Vidalia(QStringList args, int &argc, char **argv)
: QApplication(argc, argv)
{
  qInstallMsgHandler(qt_msg_handler);

  /* Read in all our command-line arguments. */
  parseArguments(args);

  /* Check if we're supposed to reset our config before proceeding. */
  if (_args.contains(ARG_RESET))
    VidaliaSettings::reset();

  /* See if we should load a default configuration file. */
  if (! VidaliaSettings::settingsFileExists())
    copyDefaultSettingsFile();

  /* Handle the -loglevel and -logfile options. */
  if (_args.contains(ARG_LOGFILE))
    _log.open(_args.value(ARG_LOGFILE));
  if (_args.contains(ARG_LOGLEVEL)) {
    _log.setLogLevel(Log::stringToLogLevel(
                      _args.value(ARG_LOGLEVEL)));
    if (!_args.contains(ARG_LOGFILE))
      _log.open(stdout);
  }
  if (!_args.contains(ARG_LOGLEVEL) && 
      !_args.contains(ARG_LOGFILE))
    _log.setLogLevel(Log::Off);

  /* Translate the GUI to the appropriate language. */
  setLanguage(_args.value(ARG_LANGUAGE));
  /* Set the GUI style appropriately. */
  setStyle(_args.value(ARG_GUISTYLE));

  /* Creates a TorControl object, used to talk to Tor. */
  _torControl = new TorControl();

#ifdef USE_MARBLE
  /* Tell Marble where to stash its generated data */
  Marble::MarbleDirs::setMarbleDataPath(dataDirectory());

#ifdef Q_OS_WIN32
  Marble::MarbleDirs::setMarblePluginPath(vApp->applicationDirPath() 
                                            + "/plugins/marble");
#endif
#endif
}

/** Destructor */
Vidalia::~Vidalia()
{
  delete _torControl;
}

/** Enters the main event loop and waits until exit() is called. The signal
 * running() will be emitted when the event loop has started. */
int
Vidalia::run()
{
  QTimer::singleShot(0, vApp, SLOT(onEventLoopStarted()));
  return vApp->exec();
}

/** Called when the application's main event loop has started. This method
 * will emit the running() signal to indicate that the application's event
 * loop is running. */
void
Vidalia::onEventLoopStarted()
{
  emit running();
}

#if defined(Q_OS_WIN)
/** On Windows, we need to catch the WM_QUERYENDSESSION message
 * so we know that it is time to shutdown. */
bool
Vidalia::winEventFilter(MSG *msg, long *result)
{
  if (msg->message == WM_QUERYENDSESSION) {
    emit shutdown();
  }
  return QApplication::winEventFilter(msg, result);
}
#endif

/** Returns true if the user wants to see usage information. */
bool
Vidalia::showUsage()
{
  return _args.contains(ARG_HELP);
}

/** Displays usage information for command-line args. */
void
Vidalia::showUsageMessageBox()
{
  QString usage;
  QTextStream out(&usage);

  out << "Available Options:" << endl;
  out << "<table>";
  out << trow(tcol("-"ARG_HELP) + 
              tcol(tr("Displays this usage message and exits.")));
  out << trow(tcol("-"ARG_RESET) +
              tcol(tr("Resets ALL stored Vidalia settings.")));
  out << trow(tcol("-"ARG_DATADIR" &lt;dir&gt;") +
              tcol(tr("Sets the directory Vidalia uses for data files.")));
  out << trow(tcol("-"ARG_PIDFILE" &lt;file&gt;") +
              tcol(tr("Sets the name and location of Vidalia's pidfile.")));
  out << trow(tcol("-"ARG_LOGFILE" &lt;file&gt;") +
              tcol(tr("Sets the name and location of Vidalia's logfile.")));
  out << trow(tcol("-"ARG_LOGLEVEL" &lt;level&gt;") +
              tcol(tr("Sets the verbosity of Vidalia's logging.") +
                   "<br>[" + Log::logLevels().join("|") +"]"));
  out << trow(tcol("-"ARG_GUISTYLE" &lt;style&gt;") +
              tcol(tr("Sets Vidalia's interface style.") +
                   "<br>[" + QStyleFactory::keys().join("|") + "]"));
  out << trow(tcol("-"ARG_LANGUAGE" &lt;language&gt;") + 
              tcol(tr("Sets Vidalia's language.") +
                   "<br>[" + LanguageSupport::languageCodes().join("|") + "]"));
  out << "</table>";

  VMessageBox::information(0, 
    tr("Vidalia Usage Information"), usage, VMessageBox::Ok);
}

/** Returns true if the specified argument expects a value. */
bool
Vidalia::argNeedsValue(QString argName)
{
  return (argName == ARG_GUISTYLE ||
          argName == ARG_LANGUAGE ||
          argName == ARG_DATADIR  ||
          argName == ARG_PIDFILE  ||
          argName == ARG_LOGFILE  ||
          argName == ARG_LOGLEVEL);
}

/** Parses the list of command-line arguments for their argument names and
 * values. */
void
Vidalia::parseArguments(QStringList args)
{
  QString arg, value;

  /* Loop through all command-line args/values and put them in a map */
  for (int i = 0; i < args.size(); i++) {
    /* Get the argument name and set a blank value */
    arg   = args.at(i).toLower();
    value = "";

    /* Check if it starts with a - or -- */
    if (arg.startsWith("-")) {
      arg = arg.mid((arg.startsWith("--") ? 2 : 1));
    }
    /* Check if it takes a value and there is one on the command-line */
    if (i < args.size()-1 && argNeedsValue(arg)) {
      value = args.at(++i);
    }
    /* Place this arg/value in the map */
    _args.insert(arg, value);
  }
}

/** Verifies that all specified arguments were valid. */
bool
Vidalia::validateArguments(QString &errmsg)
{
  /* Check for a language that Vidalia recognizes. */
  if (_args.contains(ARG_LANGUAGE) &&
      !LanguageSupport::isValidLanguageCode(_args.value(ARG_LANGUAGE))) {
    errmsg = tr("Invalid language code specified: ") + _args.value(ARG_LANGUAGE);
    return false;
  }
  /* Check for a valid GUI style */
  if (_args.contains(ARG_GUISTYLE) &&
      !QStyleFactory::keys().contains(_args.value(ARG_GUISTYLE),
                                      Qt::CaseInsensitive)) {
    errmsg = tr("Invalid GUI style specified: ") + _args.value(ARG_GUISTYLE);
    return false;
  }
  /* Check for a valid log level */
  if (_args.contains(ARG_LOGLEVEL) &&
      !Log::logLevels().contains(_args.value(ARG_LOGLEVEL))) {
    errmsg = tr("Invalid log level specified: ") + _args.value(ARG_LOGLEVEL);
    return false;
  }
  /* Check for a writable log file */
  if (_args.contains(ARG_LOGFILE) && !_log.isOpen()) {
    errmsg = tr("Unable to open log file '%1': %2")
                           .arg(_args.value(ARG_LOGFILE))
                           .arg(_log.errorString());
    return false;
  }
  return true;
}

/** Sets the translation Vidalia will use. If one was specified on the
 * command-line, we will use that. Otherwise, we'll check to see if one was
 * saved previously. If not, we'll default to one appropriate for the system
 * locale. */
bool
Vidalia::setLanguage(QString languageCode)
{
  /* If the language code is empty, use the previously-saved setting */
  if (languageCode.isEmpty()) {
    VidaliaSettings settings;
    languageCode = settings.getLanguageCode();
  }
  /* Translate into the desired langauge */
  if (retranslateUi(languageCode)) {
    _language = languageCode;
    return true;
  }
  return false;
}

/** Sets the GUI style Vidalia will use. If one was specified on the
 * command-line, we will use that. Otherwise, we'll check to see if one was
 * saved previously. If not, we'll default to one appropriate for the
 * operating system. */
bool
Vidalia::setStyle(QString styleKey)
{
  /* If no style was specified, use the previously-saved setting */
  if (styleKey.isEmpty()) {
    VidaliaSettings settings;
    styleKey = settings.getInterfaceStyle();
  }
  /* Apply the specified GUI style */
  if (QApplication::setStyle(styleKey)) {
    _style = styleKey;
    return true;
  }
  return false;
}

/** Returns the directory Vidalia uses for its data files. */
QString
Vidalia::dataDirectory()
{
  if (_args.contains(ARG_DATADIR)) {
    return _args.value(ARG_DATADIR);
  }
  return defaultDataDirectory();
}

/** Returns the default location of Vidalia's data directory. */
QString
Vidalia::defaultDataDirectory()
{
#if defined(Q_OS_WIN32)
  return (win32_app_data_folder() + "\\Vidalia");
#elif defined(Q_OS_MAC)
  return (QDir::homePath() + "/Library/Vidalia");
#else
  return (QDir::homePath() + "/.vidalia");
#endif
}

/** Returns the location of Vidalia's pid file. */
QString
Vidalia::pidFile()
{
  if (_args.contains(ARG_PIDFILE)) {
    return _args.value(ARG_PIDFILE);
  }
  return QDir::convertSeparators(dataDirectory() + "/vidalia.pid");
}

bool
Vidalia::readPasswordFromStdin()
{
  return _args.contains(ARG_READ_PASSWORD_FROM_STDIN);
}

/** Writes <b>msg</b> with severity <b>level</b> to Vidalia's log. */
Log::LogMessage
Vidalia::log(Log::LogLevel level, QString msg)
{
  return _log.log(level, msg);
}

/** Creates and binds a shortcut such that when <b>key</b> is pressed in
 * <b>sender</b>'s context, <b>receiver</b>'s <b>slot</b> will be called. */
void
Vidalia::createShortcut(const QKeySequence &key, QWidget *sender,
                        QObject *receiver, const char *slot)
{
  QShortcut *s = new QShortcut(key, sender);
  connect(s, SIGNAL(activated()), receiver, slot);
}

/** Creates and binds a shortcut such that when <b>key</b> is pressed in
 * <b>sender</b>'s context, <b>receiver</b>'s <b>slot</b> will be called. */
void
Vidalia::createShortcut(const QString &key, QWidget *sender,
                        QObject *receiver, const char *slot)
{
  createShortcut(QKeySequence(key), sender, receiver, slot);
}

void
Vidalia::removeAllTranslators()
{
  foreach (QTranslator *translator, _translators) {
    QApplication::removeTranslator(translator);
    delete translator;
  }
  _translators.clear();
}

bool
Vidalia::retranslateUi(const QString &languageCode)
{
  QTranslator *systemQtTranslator = 0;
  QTranslator *vidaliaQtTranslator = 0;
  QTranslator *vidaliaTranslator = 0;

  if (! LanguageSupport::isValidLanguageCode(languageCode))
    return false;
  if (! languageCode.compare("en", Qt::CaseInsensitive)) {
    _language = languageCode;
    removeAllTranslators();
    return true;
  }

  systemQtTranslator = new QTranslator(vApp);
  Q_CHECK_PTR(systemQtTranslator);
  QString qtDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
  systemQtTranslator->load(qtDir + "/qt_" + languageCode + ".qm");


  vidaliaQtTranslator = new QTranslator(vApp);
  Q_CHECK_PTR(vidaliaQtTranslator);
  vidaliaQtTranslator->load(":/lang/qt_" + languageCode + ".qm");

  vidaliaTranslator = new QTranslator(vApp);
  Q_CHECK_PTR(vidaliaTranslator);
  if (! vidaliaTranslator->load(":/lang/vidalia_" + languageCode + ".qm"))
    goto err;

  removeAllTranslators();
  _language = languageCode;
  QApplication::installTranslator(systemQtTranslator);
  QApplication::installTranslator(vidaliaQtTranslator);
  QApplication::installTranslator(vidaliaTranslator);
  _translators << systemQtTranslator
               << vidaliaQtTranslator
               << vidaliaTranslator;
  return true;

err:
  if (systemQtTranslator)
    delete systemQtTranslator;
  if (vidaliaQtTranslator)
    delete vidaliaQtTranslator;
  if (vidaliaTranslator)
    delete vidaliaTranslator;
  delete vidaliaTranslator;
  return false;
}

/** Copies a default settings file (if one exists) to Vidalia's data
 * directory. */
void
Vidalia::copyDefaultSettingsFile() const
{
#ifdef Q_OS_MACX
  CFURLRef confUrlRef;
  CFStringRef pathRef;
  const char *path;

  confUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), 
                                       CFSTR("vidalia"), CFSTR("conf"), NULL);
  if (confUrlRef == NULL)
    return;

  pathRef = CFURLCopyFileSystemPath(confUrlRef, kCFURLPOSIXPathStyle);
  path    = CFStringGetCStringPtr(pathRef, CFStringGetSystemEncoding());

  if (path) {
    QString defaultConfFile = QString::fromLocal8Bit(path);
    QFileInfo fi(defaultConfFile);
    if (fi.exists())
      QFile::copy(defaultConfFile, VidaliaSettings::settingsFile());
  }
  CFRelease(confUrlRef);
  CFRelease(pathRef);
#endif
}

