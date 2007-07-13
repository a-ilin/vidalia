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
 * \file vidalia.cpp
 * \version $Id$
 * \brief Main Vidalia QApplication object
 */

#include <QDir>
#include <QTextStream>
#include <QStyleFactory>
#include <util/string.h>
#include <lang/languagesupport.h>
#include <gui/common/vmessagebox.h>
#include <util/html.h>
#include <stdlib.h>

#include "vidalia.h"

/* Available command-line arguments. */
#define ARG_LANGUAGE   "lang"     /**< Argument specifying language.    */
#define ARG_GUISTYLE   "style"    /**< Argument specfying GUI style.    */
#define ARG_RESET      "reset"    /**< Reset Vidalia's saved settings.  */
#define ARG_HELP       "help"     /**< Display usage informatino.       */
#define ARG_DATADIR    "datadir"  /**< Directory to use for data files. */
#define ARG_PIDFILE    "pidfile"  /**< Location and name of our pidfile.*/
#define ARG_LOGFILE    "logfile"  /**< Location of our logfile.         */
#define ARG_LOGLEVEL   "loglevel" /**< Log verbosity.                   */


/* Static member variables */
QMap<QString, QString> Vidalia::_args; /**< List of command-line arguments.  */
QString Vidalia::_style;               /**< The current GUI style.           */
QString Vidalia::_language;            /**< The current language.            */
HelpBrowser*  Vidalia::_help = 0;      /**< Vidalia's help system.           */
TorControl* Vidalia::_torControl = 0;  /**< Main TorControl object.          */
Log Vidalia::_log;

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
}

/** Destructor */
Vidalia::~Vidalia()
{
  if (_help)
    delete _help;
  delete _torControl;
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
  if (LanguageSupport::translate(languageCode)) {
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

/** Displays the help page associated with the specified topic. If no topic is
 * specified, then the default help page will be displayed. */
void
Vidalia::help(QString topic)
{
  if (!_help)
    _help = new HelpBrowser();
  _help->showWindow(topic);
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

Log::LogMessage
Vidalia::log(Log::LogLevel level, QString msg)
{
  return _log.log(level, msg);
}

