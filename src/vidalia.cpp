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
 * \file vidalia.cpp
 * \version $Id$
 */

#include <QTextStream>
#include <QStyleFactory>

#include <lang/languagesupport.h>

#include "vidalia.h"

/* Available command-line arguments. */
#define ARG_LANGUAGE   "lang"  /**< Argument specifying language.    */
#define ARG_GUISTYLE   "style" /**< Argument specfying GUI style.    */
#define ARG_RESET      "reset" /**< Reset Vidalia's saved settings.  */
#define ARG_HELP       "help"  /**< Display usage informatino.       */


/* Static member variables */
QMap<QString, QString> Vidalia::_args; /**< List of command-line arguments.  */
QString Vidalia::_style;               /**< The current GUI style.           */
QString Vidalia::_language;            /**< The current language.            */
VidaliaSettings Vidalia::_settings;    /**< Vidalia's configurable settings. */
HelpBrowser*  Vidalia::_help = 0;      /**< Vidalia's help system.           */
TorControl* Vidalia::_torControl = 0;  /**< Main TorControl object.          */


/** Constructor. Parses the command-line arguments, resets Vidalia's
 * configuration (if requested), and sets up the GUI style and language
 * translation. */
Vidalia::Vidalia(QStringList args, int &argc, char **argv)
: QApplication(argc, argv)
{
  /* Read in all our command-line arguments. */
  parseArguments(args);

  /* Check if we're supposed to reset our config before proceeding. */
  if (_args.contains(ARG_RESET)) {
    _settings.reset();
  }

  /** Initialize support for language translations. */
  LanguageSupport::initialize();

  /** Translate the GUI to the appropriate language. */
  setLanguage(_args.value(ARG_LANGUAGE));

  /** Set the GUI style appropriately. */
  setStyle(_args.value(ARG_GUISTYLE));

  /** Creates a TorControl object, used to talk to Tor. */
  _torControl = new TorControl();
  /** Create a help browser object, used to dispaly various help topics. */
  _help = new HelpBrowser();
}

/** Destructor */
Vidalia::~Vidalia()
{
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

/** Display usage information regarding command-line arguments. */
void
Vidalia::printUsage(QString errmsg)
{
  QTextStream out(stdout);

  /* If there was an error message, print it out. */
  if (!errmsg.isEmpty()) {
    out << "** " << errmsg << " **" << endl << endl;
  }

  /* Now print the application usage */
  out << "Usage: " << endl;
  out << "\t" << qApp->arguments().at(0) << " [options]"    << endl;

  /* And available options */
  out << endl << "Available Options:"                                   << endl;
  out << "\t-"ARG_HELP"\t\tDisplays this usage message and exits."      << endl;
  out << "\t-"ARG_RESET"\t\tResets ALL stored Vidalia settings."        << endl;
  out << "\t-"ARG_GUISTYLE"\t\tSets Vidalia's interface style."         << endl;
  out << "\t\t\t[" << QStyleFactory::keys().join("|") << "]"            << endl;
  out << "\t-"ARG_LANGUAGE"\t\tSets Vidalia's language."                << endl;
  out << "\t\t\t[" << LanguageSupport::languageCodes().join("|") << "]" << endl;
}

/** */
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
    if (i < args.size()-1 && (arg == ARG_GUISTYLE || arg == ARG_LANGUAGE)) {
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
  /* If they want help, just return false now */
  if (_args.contains(ARG_HELP)) {
    return false;
  }
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
    languageCode = _settings.getLanguageCode();
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
    styleKey = _settings.getInterfaceStyle();
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
  _help->show(topic);
}

