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
 * \file torsettings.cpp
 * \version $Id$
 */

#include <QDir>

#include "torsettings.h"

/* Tor Settings */
#define SETTING_TOR_PATH       "Tor/TorPath"
#define SETTING_TOR_ARGUMENTS  "Tor/Arguments"
#define SETTING_CONTROL_ADDR   "Tor/ControlAddr"
#define SETTING_CONTROL_PORT   "Tor/ControlPort"
#define SETTING_AUTH_TOKEN     "Tor/AuthToken"

/* Default Tor Location */
#if defined(Q_OS_WIN32)
#include <util/win32.h>
#define DEFAULT_TOR_PATH       (win32_program_files_folder() + "\\Tor")
#elif defined(Q_OS_MACX)
#define DEFAULT_TOR_PATH       "/usr/bin"
#else
#define DEFAULT_TOR_PATH       "/usr/local/bin"
#endif

/* On win32, we need to add the .exe onto Tor's filename */
#ifdef Q_OS_WIN32
#define TOR_EXECUTABLE    "tor.exe"
#else
#define TOR_EXECUTABLE    "tor"
#endif

/* Default Tor Settings */
#define DEFAULT_TOR_ARGUMENTS   QMap<QString,QVariant>()
#define DEFAULT_CONTROL_ADDR    "127.0.0.1"
#define DEFAULT_CONTROL_PORT    9051
#define DEFAULT_AUTH_TOKEN      ""
#define DEFAULT_TORRC           ""
#define DEFAULT_USER            ""
#define DEFAULT_GROUP           ""

/* Arguments we can pass to Tor on the command-line */
#define TOR_ARG_CONTROL_PORT    "ControlPort"
#define TOR_ARG_TORRC           "-f"
#define TOR_ARG_RUN_AS_USER     "User"
#define TOR_ARG_RUN_AS_GROUP    "Group"


/** Default constructor */
TorSettings::TorSettings()
{
}

/** Get the path to Tor's executable from Vidalia's configuration. If a path
* hasn't been specified, then default to the current directory.
*/
QString
TorSettings::getPath()
{
  return QDir::convertSeparators(value(SETTING_TOR_PATH,
                                       DEFAULT_TOR_PATH).toString()); 
}

/** Set the path to Tor's executable */
void
TorSettings::setPath(QString path)
{
  setValue(SETTING_TOR_PATH, path);
}

/** Returns a fully-qualified path to Tor's executable, including the
 * executable name. */
QString
TorSettings::getExecutable()
{
  return QDir::convertSeparators(QFileInfo(getPath(), 
                                           TOR_EXECUTABLE).absoluteFilePath());
}

/** Returns a formatted QString of all currently set command-line arguments.
 * If an argument's value contains a space, then it will be wrapped in quotes.
 * */
QString
TorSettings::getArguments()
{ 
  QMap<QString, QVariant> args = getArgumentsMap();
  QString fmtArgs, arg;
  foreach(QString key, args.keys()) {
    arg = args.value(key).toString();
    if (arg.indexOf(" ") > 0) {
      arg = "\"" + arg + "\"";
    }
    fmtArgs.append(" " + key + " " + arg);
  }
  return fmtArgs;
}

/** Returns a QMap<arg, value> containing all arguments to be passed to Tor's
* executable. */
QMap<QString, QVariant>
TorSettings::getArgumentsMap()
{
  QMap<QString, QVariant> args = value(SETTING_TOR_ARGUMENTS,
                                       DEFAULT_TOR_ARGUMENTS).toMap();
  args.insert(TOR_ARG_CONTROL_PORT, QString::number(getControlPort()));
  return args;
}

/** Returns true if we have a setting for the given Tor argument. */
bool
TorSettings::hasArgument(QString arg)
{
  return getArgumentsMap().contains(arg);
}

/** Set Tor arguments to the supplied arg-value map. */
void
TorSettings::setArguments(QMap<QString, QVariant> args)
{
  setValue(SETTING_TOR_ARGUMENTS, args);
}

/** Add an argument to the list of command-line arguments used when starting
* Tor. */
void
TorSettings::setArgument(QString arg, QString value)
{
  QMap<QString, QVariant> args = getArgumentsMap();
  args.insert(arg, value);
  setArguments(args);
}

/** Gets the stored value for the specified argument name. If no value is set,
 * then defaultValue is returned. */
QVariant
TorSettings::getArgument(QString arg, QVariant defaultValue)
{
  return getArgumentsMap().value(arg, defaultValue);
}

/** Remove a single Tor command-line argument. */
void
TorSettings::unsetArgument(QString arg)
{
  QMap<QString, QVariant> args = getArgumentsMap();
  args.remove(arg);
  setArguments(args);
}

/** Returns the torrc that will be used when starting Tor. If the default
 * torrc is used, then this method returns an empty string. */
QString
TorSettings::getTorrc()
{
  return getArgument(TOR_ARG_TORRC, DEFAULT_TORRC).toString();
}

/** Sets the torrc that will be used when starting Tor.
 * \param torrc The torrc to use. 
 */
void
TorSettings::setTorrc(QString torrc)
{
  torrc.isEmpty() ? unsetArgument(TOR_ARG_TORRC)
                  : setArgument(TOR_ARG_TORRC, torrc);
}

/** Returns true if Tor is started using an alternate torrc file. */
bool
TorSettings::usingAlternateTorrc()
{
  return hasArgument(TOR_ARG_TORRC);
}

/** Returns the user used when running Tor. The user is specified as an
 * argument to Tor, which will setuid to this user. */
QString
TorSettings::getUser()
{
  return getArgument(TOR_ARG_RUN_AS_USER, DEFAULT_USER).toString();
}

/** Sets the user used when running Tor. The user is specified as an argument
 * to Tor, which will setuid to this user. */
void
TorSettings::setUser(QString user)
{
  user.isEmpty() ? unsetArgument(TOR_ARG_RUN_AS_USER)
                 : setArgument(TOR_ARG_RUN_AS_USER, user);
}

/** Returns the group used when running Tor. The group is specified as an
 * argument to Tor, which will setgid to this group. */
QString
TorSettings::getGroup()
{
  return getArgument(TOR_ARG_RUN_AS_GROUP, DEFAULT_GROUP).toString();
}

/** Sets the group used when running Tor. The group is specified as an
 * argument to Tor, which will setgid to this group. */
void
TorSettings::setGroup(QString group)
{
  group.isEmpty() ? unsetArgument(TOR_ARG_RUN_AS_GROUP)
                  : setArgument(TOR_ARG_RUN_AS_GROUP, group);
}

/** Get the address or hostname used to connect to Tor */
QHostAddress
TorSettings::getControlAddress()
{
  QString addr = value(SETTING_CONTROL_ADDR,
                       DEFAULT_CONTROL_ADDR).toString();
  return QHostAddress(addr);
}

/** Set the address or hostname used to connect to Tor */
void
TorSettings::setControlAddress(QHostAddress addr)
{
  setValue(SETTING_CONTROL_PORT, addr.toString());
}

/** Get the control port used to connect to Tor */
quint16
TorSettings::getControlPort()
{
  return (quint16)value(SETTING_CONTROL_PORT, 
                        DEFAULT_CONTROL_PORT).toInt();
}

/** Set the control port used to connect to Tor */
void
TorSettings::setControlPort(quint16 port)
{
  setValue(SETTING_CONTROL_PORT, port);
}

/** Get the authentication token sent by the controller to Tor. For now, this
* just sends the default (blank) authentication token. It is implemented as a
* stub here to make it easy to do real authentication in the future. */
QByteArray
TorSettings::getAuthToken()
{
  return QByteArray::fromBase64(value(SETTING_AUTH_TOKEN,
                                      QByteArray(DEFAULT_AUTH_TOKEN)).toByteArray());
}

/** Set the authentication token sent by the controller to Tor. */
void
TorSettings::setAuthToken(QByteArray token)
{
  setValue(SETTING_AUTH_TOKEN, token.toBase64());
}

