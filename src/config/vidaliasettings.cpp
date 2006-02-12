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

#define SETTING_MSG_FILTER      "MessageLog/MessageFilter"
#define SETTING_MAX_MESSAGE     "MessageLog/MaxMsgCount"
#define SETTING_MSGLOG_OPACITY  "MessageLog/Opacity"
#define SETTING_ENABLE_LOG_FILE "MessageLog/EnableLogFile"
#define SETTING_LOG_FILE        "MessageLog/LogFile"

#define SETTING_BWGRAPH_FILTER      "BandwidthGraph/BWLineFilter"
#define SETTING_BWGRAPH_OPACITY     "BandwidthGraph/Opacity"

/* Default Tor Settings */
#if defined(Q_OS_WIN32)
#define DEFAULT_TOR_PATH       (QDir::rootPath() + "\\Program Files\\Tor")
#elif defined(Q_OS_MACX)
#define DEFAULT_TOR_PATH       "/usr/bin"
#else
#define DEFAULT_TOR_PATH       "/usr/local/bin"
#endif
#define DEFAULT_OPACITY 100

#define DEFAULT_TOR_ARGUMENTS  "-ControlPort 9051"
#define DEFAULT_CONTROL_ADDR   "127.0.0.1"
#define DEFAULT_CONTROL_PORT   9051
#define DEFAULT_AUTH_TOKEN     ""

/* Default message log settings */
#define DEFAULT_MSG_FILTER     (TOR_ERROR|TOR_WARN|TOR_NOTICE)
#define DEFAULT_MAX_MESSAGE    500
#define DEFAULT_ENABLE_LOG_FILE false

#if defined(Q_OS_WIN32)
#define DEFAULT_LOG_FILE       (QDir::rootPath() + "\\Program Files\\Tor\\tor.log")
#else
#define DEFAULT_LOG_FILE       (QDir::homePath() + "/.tor/tor.log")
#endif

/* Default bandwidth graph settings */
#define DEFAULT_BWGRAPH_FILTER  (BWGRAPH_SEND|BWGRAPH_REC)

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
QString
VidaliaSettings::getTorPath()
{
  return QDir::convertSeparators(value(SETTING_TOR_PATH,
                                       DEFAULT_TOR_PATH).toString()); 
}

/** Set the path to Tor's executable */
void
VidaliaSettings::setTorPath(QString path)
{
  setValue(SETTING_TOR_PATH, path);
}

/** Returns a fully-qualified path to Tor's executable */
QString
VidaliaSettings::getTorExecutable()
{
  return QDir::convertSeparators(
           QFileInfo(getTorPath(), TOR_EXECUTABLE).absoluteFilePath());
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

/** Returns the current message filter. */
uint
VidaliaSettings::getMsgFilter()
{
  return value(SETTING_MSG_FILTER, DEFAULT_MSG_FILTER).toUInt(); 
}

/** Saves the setting for whether or not the given message severity will be
 * displayed. */
void
VidaliaSettings::setMsgFilter(LogEvent::Severity severity, bool status)
{
  uint filter = getMsgFilter();
  filter = (status ? (filter | (uint)severity) : (filter & ~((uint)severity)));
  setValue(SETTING_MSG_FILTER, filter);
}

/** 
 Set maximum number of messages to display in log
*/
void
VidaliaSettings::setMaxMsgCount(int max)
{
  setValue(SETTING_MAX_MESSAGE, max);
}

/**
 Return maximum number of messages to display in log
**/
int
VidaliaSettings::getMaxMsgCount()
{
  return value(SETTING_MAX_MESSAGE, DEFAULT_MAX_MESSAGE).toInt();
}

/**
 Get the level of opacity for the MessageLog window
**/
int
VidaliaSettings::getMsgLogOpacity()
{
  return value(SETTING_MSGLOG_OPACITY, DEFAULT_OPACITY).toInt();
}

/**
 Set the level of opacity for the MessageLog window
**/
void
VidaliaSettings::setMsgLogOpacity(int value)
{
  setValue(SETTING_MSGLOG_OPACITY, value);
}

/** Returns whether or not to enable automatically saving log messages from
 * the message log to a file on disk. */
bool
VidaliaSettings::isLogFileEnabled()
{
  return value(SETTING_ENABLE_LOG_FILE, 
               DEFAULT_ENABLE_LOG_FILE).toBool();
}

/** Sets whether or not to enable automatically saving log messages from the
 * message log to a file on disk. */
void
VidaliaSettings::enableLogFile(bool enable)
{
  setValue(SETTING_ENABLE_LOG_FILE, enable);
}

/** Gets the destination file on disk to which log messages can be saved. */
QString
VidaliaSettings::getLogFile()
{
  return QDir::convertSeparators(
                  value(SETTING_LOG_FILE, DEFAULT_LOG_FILE).toString());
}

/** Sets the destination file on disk to which log messages can be saved. */
void
VidaliaSettings::setLogFile(QString file)
{
  setValue(SETTING_LOG_FILE, QDir::convertSeparators(file));
}

/** Returns the bandwidth line filter. */
uint
VidaliaSettings::getBWGraphFilter()
{
  return value(SETTING_BWGRAPH_FILTER, DEFAULT_BWGRAPH_FILTER).toUInt(); 
}

/** Saves the setting for whether or not the given line will be graphed */
void
VidaliaSettings::setBWGraphFilter(uint line, bool status)
{
  uint filter = getBWGraphFilter();
  filter = (status ? (filter | line) : (filter & ~(line)));
  setValue(SETTING_BWGRAPH_FILTER, filter);
}

/**
 Get the level of opacity for the BandwidthGraph window
**/
int
VidaliaSettings::getBWGraphOpacity()
{
  return value(SETTING_BWGRAPH_OPACITY, DEFAULT_OPACITY).toInt();
}

/**
 Set the level of opacity for the BandwidthGraph window
**/
void
VidaliaSettings::setBWGraphOpacity(int value)
{
  setValue(SETTING_BWGRAPH_OPACITY, value);
}
