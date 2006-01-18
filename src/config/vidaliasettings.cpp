/****************************************************************
 *  $Id$
 * 
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

#include "vidaliasettings.h"

/* Vidalia's version string */
#define VIDALIA_VERSION        "0.0.1-alpha"

/* Vidalia's Settings */
#define SETTING_TOR_PATH       "Tor/TorPath"
#define SETTING_TOR_ARGUMENTS  "Tor/Arguments"
#define SETTING_CONTROL_ADDR   "Tor/ControlAddr"
#define SETTING_CONTROL_PORT   "Tor/ControlPort"
#define SETTING_AUTH_TOKEN     "Tor/AuthToken"
#define SETTING_SHOW_TORERR    "Tor/ShowTorErr"
#define SETTING_SHOW_TORWARN   "Tor/ShowTorWarn"
#define SETTING_SHOW_TORNOTE   "Tor/ShowTorNote"
#define SETTING_SHOW_TORINFO   "Tor/ShowTorInfo"
#define SETTING_SHOW_TORDEBUG  "Tor/ShowTorDebug"
#define SETTING_SHOW_VIDERR    "Tor/ShowVidError"
#define SETTING_SHOW_VIDSTAT   "Tor/ShowVidStat"
#define SETTING_MAX_MESSAGE    "Tor/MaxMsgCount"

/* Default Settings */
#if defined(Q_OS_WIN32)
#define DEFAULT_TOR_PATH       "C:\\Program Files\\Tor"
#elif defined(Q_OS_MACX)
#define DEFAULT_TOR_PATH       "/usr/bin"
#else
#define DEFAULT_TOR_PATH       "/usr/local/bin"
#endif

#define DEFAULT_TOR_ARGUMENTS  "-ControlPort 9051"
#define DEFAULT_CONTROL_ADDR   "127.0.0.1"
#define DEFAULT_CONTROL_PORT   9051
#define DEFAULT_AUTH_TOKEN     ""

#define DEFAULT_SHOW_TORERR    true
#define DEFAULT_SHOW_TORWARN   true
#define DEFAULT_SHOW_TORNOTE   true
#define DEFAULT_SHOW_TORINFO   false
#define DEFAULT_SHOW_TORDEBUG  false
#define DEFAULT_SHOW_VIDERR    true
#define DEFAULT_SHOW_VIDSTAT   true

#define DEFAULT_MAX_MESSAGE    500

/** Default Constructor
 * We use "Vidalia" for both the company name and the application name.
 */
VidaliaSettings::VidaliaSettings()
  : QSettings("vidalia", "vidalia")
{
}

/** Default Destructor */
VidaliaSettings::~VidaliaSettings()
{
}

/** Return Vidalia's version string */
QString
VidaliaSettings::getVersion()
{
  return QString(VIDALIA_VERSION);
}

/** Get the path to Tor's executable from Vidalia's configuration. If a path
 * hasn't been specified, then default to the current directory.
 */
QDir
VidaliaSettings::getTorPath()
{
  QString path = value(SETTING_TOR_PATH, DEFAULT_TOR_PATH).toString(); 
  return QDir(path);
}

/** Set the path to Tor's executable */
void
VidaliaSettings::setTorPath(QDir path)
{
  setValue(SETTING_TOR_PATH, path.absolutePath());
}

/** Returns a fully-qualified path to Tor's executable */
QFileInfo
VidaliaSettings::getTorExecutable()
{
  return QFileInfo(getTorPath(), TOR_EXECUTABLE);
}

/** Returns a QStringList containing all arguments to be passed to Tor's
 * executable. Arguments are stored in the settings file as a ;-delimited
 * list. */
QStringList
VidaliaSettings::getTorArguments()
{
  QString args = value(SETTING_TOR_ARGUMENTS,
                       DEFAULT_TOR_ARGUMENTS).toString();
  return args.split(";");
}

/** Set Tor arguments to a ;-delimited list of all arguments in the supplied
 * QStringList. */
void
VidaliaSettings::setTorArguments(QStringList args)
{
  setValue(SETTING_TOR_ARGUMENTS, args.join(";"));
}

/** Add an argument to the list of command-line arguments used when starting
 * Tor. */
void
VidaliaSettings::addTorArgument(QString arg)
{
  QStringList args = getTorArguments();
  args << arg;
  setTorArguments(args);
}

/** Remove a single Tor command-line argument. */
void
VidaliaSettings::removeTorArgument(QString arg)
{
  QStringList args = getTorArguments();

  /* QStringList doesn't appear to have a case-insensitive find()-esque
   * method, so we'll do it the hard way */
  for (int i = 0; i < args.size(); ++i) {
    if (args.at(i).toLower() == arg.toLower()) {
      args.removeAt(i);
      break;
    }
  }
}

/** Get the address or hostname used to connect to Tor */
QHostAddress
VidaliaSettings::getControlAddress()
{
  QString addr = value(SETTING_CONTROL_ADDR,
                       DEFAULT_CONTROL_ADDR).toString();
  return QHostAddress(addr);
}

/** Set the address or hostname used to connect to Tor */
void
VidaliaSettings::setControlAddress(QHostAddress addr)
{
  setValue(SETTING_CONTROL_PORT, addr.toString());
}

/** Get the control port used to connect to Tor */
quint16
VidaliaSettings::getControlPort()
{
  return (quint16)value(SETTING_CONTROL_PORT, 
                        DEFAULT_CONTROL_PORT).toInt();
}

/** Set the control port used to connect to Tor */
void
VidaliaSettings::setControlPort(quint16 port)
{
  setValue(SETTING_CONTROL_PORT, port);
}

/** Get the authentication token sent by the controller to Tor. For now, this
 * just sends the default (blank) authentication token. It is implemented as a
 * stub here to make it easy to do real authentication in the future. */
QByteArray
VidaliaSettings::getAuthToken()
{
  return QByteArray::fromBase64(value(SETTING_AUTH_TOKEN,
                                QByteArray(DEFAULT_AUTH_TOKEN)).toByteArray());
}

/** Set the authentication token sent by the controller to Tor. */
void
VidaliaSettings::setAuthToken(QByteArray token)
{
  setValue(SETTING_AUTH_TOKEN, token.toBase64());
}

/*
 Set whether or not messages of type are displayed in the log
*/
void
VidaliaSettings::setShowMsg(const char* type, bool status)
{
  if (!qstrcmp(type, MSG_TORERR)) {
    setValue(SETTING_SHOW_TORERR, status);
  
  } else if (!qstrcmp(type, MSG_TORWARN)) {
    setValue(SETTING_SHOW_TORWARN, status);
  
  } else if (!qstrcmp(type, MSG_TORNOTE)) {
    setValue(SETTING_SHOW_TORNOTE, status);
  
  } else if (!qstrcmp(type, MSG_TORINFO)) {
    setValue(SETTING_SHOW_TORINFO, status);
  
  } else if (!qstrcmp(type, MSG_TORDEBUG)) {
    setValue(SETTING_SHOW_TORDEBUG, status);
  
  } else if (!qstrcmp(type, MSG_VIDERR)) {
    setValue(SETTING_SHOW_VIDERR, status);
 
  } else if (!qstrcmp(type, MSG_VIDSTAT)) {
    setValue(SETTING_SHOW_VIDSTAT, status); 
  }
}

/*
 Returns bool indicating whether the type of message is to be shown
*/
bool
VidaliaSettings::getShowMsg(const char* type)
{
  if (!qstrcmp(type, MSG_TORERR)) {
    return value(SETTING_SHOW_TORERR, DEFAULT_SHOW_TORERR).toBool(); 
  
  } else if (!qstrcmp(type, MSG_TORWARN)) {
    return value(SETTING_SHOW_TORWARN, DEFAULT_SHOW_TORWARN).toBool();
  
  } else if (!qstrcmp(type, MSG_TORNOTE)) {
    return value(SETTING_SHOW_TORNOTE, DEFAULT_SHOW_TORNOTE).toBool();
  
  } else if (!qstrcmp(type, MSG_TORINFO)) {
    return value(SETTING_SHOW_TORINFO, DEFAULT_SHOW_TORINFO).toBool();
  
  } else if (!qstrcmp(type, MSG_TORDEBUG)) {
    return value(SETTING_SHOW_TORDEBUG, DEFAULT_SHOW_TORDEBUG).toBool();
  
  } else if (!qstrcmp(type, MSG_VIDERR)) {
    return value(SETTING_SHOW_VIDERR, DEFAULT_SHOW_VIDERR).toBool();
 
  } else if (!qstrcmp(type, MSG_VIDSTAT)) {
    return value(SETTING_SHOW_VIDSTAT, DEFAULT_SHOW_VIDSTAT).toBool(); 
  
  } else {
    return false;
  }
}

/* 
 Set maximum number of messages to display in log
*/
void
VidaliaSettings::setMaxMsgCount(int max)
{
  setValue(SETTING_MAX_MESSAGE, max);
}

/*
 Return maximum number of messages to display in log
*/
int
VidaliaSettings::getMaxMsgCount()
{
  return value(SETTING_MAX_MESSAGE, DEFAULT_MAX_MESSAGE).toInt();
}
