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
 * \brief Settings used for starting and running Tor
 */

#include <QDir>
#include <vidalia.h>

#include "torsettings.h"

/* Tor Settings */
#define SETTING_TOR_EXECUTABLE  "Tor/TorExecutable"
#define SETTING_TORRC           "Tor/Torrc"
#define SETTING_CONTROL_ADDR    "Tor/ControlAddr"
#define SETTING_CONTROL_PORT    "Tor/ControlPort"
#define SETTING_AUTH_TOKEN      "Tor/AuthToken"
#define SETTING_USE_SERVICE     "Tor/UseService"
#define SETTING_TOR_USER        "Tor/User"
#define SETTING_TOR_GROUP       "Tor/Group"

/* On win32, we need to add the .exe onto Tor's filename */
#if defined(Q_OS_WIN32)
#include <util/win32.h>
#define DEFAULT_TOR_EXECUTABLE    (win32_program_files_folder() + "\\Tor\\tor.exe")
#else
#define DEFAULT_TOR_EXECUTABLE    "tor"
#endif

/* Arguments we can pass to Tor on the command-line */
#define TOR_ARG_CONTROL_PORT    "ControlPort"
#define TOR_ARG_TORRC           "-f"
#define TOR_ARG_USER            "User"
#define TOR_ARG_GROUP           "Group"


/** Default constructor */
TorSettings::TorSettings()
{
  setDefault(SETTING_TOR_EXECUTABLE, DEFAULT_TOR_EXECUTABLE);
  setDefault(SETTING_TORRC,         Vidalia::dataDirectory() + "/torrc");
  setDefault(SETTING_CONTROL_ADDR,  "127.0.0.1");
  setDefault(SETTING_CONTROL_PORT,  9051);
  setDefault(SETTING_AUTH_TOKEN,    QByteArray(""));
  setDefault(SETTING_TOR_USER,      "");
  setDefault(SETTING_TOR_GROUP,     "");
  setDefault(SETTING_USE_SERVICE,   false);
}

/** Returns a fully-qualified path to Tor's executable, including the
 * executable name. */
QString
TorSettings::getExecutable()
{
  return QDir::convertSeparators(
            value(SETTING_TOR_EXECUTABLE).toString());
}

/** Sets the location and name of Tor's executable to the given string. */
void
TorSettings::setExecutable(QString torExecutable)
{
  setValue(SETTING_TOR_EXECUTABLE, torExecutable);
}

/** Formats the argument name <b>name</b> with the given value <b>value</b>.
 * If <b>value</b> contains a space, <b>value</b> will be wrapped in quotes. */
QString
TorSettings::formatArgument(QString name, QString value)
{
  if (value.indexOf(" ") >= 0) {
    value = "\"" + value + "\"";
  }
  return name + " " + value;
}

/** Returns a formatted QString of all currently set command-line arguments.
 * If an argument's value contains a space, then it will be wrapped in quotes.
 * */
QString
TorSettings::getArguments()
{
  QString args;

  /* Add the torrc argument (if specified) */
  QString torrc = getTorrc();
  if (!torrc.isEmpty()) {
    args += formatArgument(TOR_ARG_TORRC, torrc) + " ";
  }
  /* Add the ControlPort value */
  quint16 controlPort = getControlPort();
  if (controlPort) {
    args += formatArgument(TOR_ARG_CONTROL_PORT, 
                           QString::number(controlPort)) + " ";
  }
  /* Add the User argument (if specified) */
  QString user = getUser();
  if (!user.isEmpty()) {
    args += formatArgument(TOR_ARG_USER, user) + " ";
  }
  /* Add the Group argument (if specified) */
  QString group = getGroup();
  if (!group.isEmpty()) {
    args += formatArgument(TOR_ARG_GROUP, group);
  }
  return args;
}

/** Returns the torrc that will be used when starting Tor. */
QString
TorSettings::getTorrc()
{
  return value(SETTING_TORRC).toString();
}

/** Sets the torrc that will be used when starting Tor.
 * \param torrc The torrc to use. 
 */
void
TorSettings::setTorrc(QString torrc)
{
  setValue(SETTING_TORRC, torrc);
}

/** Returns the user used when running Tor. The user is specified as an
 * argument to Tor, which will setuid to this user. */
QString
TorSettings::getUser()
{
  return value(SETTING_TOR_USER).toString();
}

/** Sets the user used when running Tor. The user is specified as an argument
 * to Tor, which will setuid to this user. */
void
TorSettings::setUser(QString user)
{
  setValue(SETTING_TOR_USER, user);
}

/** Returns the group used when running Tor. The group is specified as an
 * argument to Tor, which will setgid to this group. */
QString
TorSettings::getGroup()
{
  return value(SETTING_TOR_GROUP).toString();
}

/** Sets the group used when running Tor. The group is specified as an
 * argument to Tor, which will setgid to this group. */
void
TorSettings::setGroup(QString group)
{
  setValue(SETTING_TOR_GROUP, group);
}

/** Get the address or hostname used to connect to Tor */
QHostAddress
TorSettings::getControlAddress()
{
  QString addr = value(SETTING_CONTROL_ADDR).toString();
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
  return (quint16)value(SETTING_CONTROL_PORT).toInt();
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
  return QByteArray::fromBase64(
            value(SETTING_AUTH_TOKEN).toByteArray());
}

/** Set the authentication token sent by the controller to Tor. */
void
TorSettings::setAuthToken(QByteArray token)
{
  setValue(SETTING_AUTH_TOKEN, token.toBase64());
}

/** Get whether Tor will run as an NT service */
bool
TorSettings::getUseService()
{
  return value(SETTING_USE_SERVICE).toBool();
}

/** Set whether Tor will run as an NT service */
void
TorSettings::setUseService(bool useService)
{
  setValue(SETTING_USE_SERVICE, useService);
}

