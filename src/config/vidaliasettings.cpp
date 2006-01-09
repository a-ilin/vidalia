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

#include <QCoreApplication>

#include "vidaliasettings.h"

/* Vidalia's Settings */
#define SETTING_TOR_PATH       "Tor/TorPath"
#define SETTING_CONTROL_PORT   "Tor/ControlPort"

/* Default Settings */
#define DEFAULT_CONTROL_PORT   9051


/** Default Constructor
 * We use "Vidalia" for both the company name and the application name.
 */
VidaliaSettings::VidaliaSettings()
  : QSettings("Vidalia", "Vidalia")
{
}

/** Default Destructor */
VidaliaSettings::~VidaliaSettings()
{
}

/** Get the path to Tor's executable from Vidalia's configuration. If a path
 * hasn't been specified, then default to the current directory.
 */
QString
VidaliaSettings::getTorPath()
{
  return value(SETTING_TOR_PATH, 
               QCoreApplication::applicationDirPath()).toString();
}

/** Set the path to Tor's executable */
void
VidaliaSettings::setTorPath(QString path)
{
  setValue(SETTING_TOR_PATH, path);
}

/** Returns a fully-qualified path to Tor's executable, including the
 * executable name. (and adds a ".exe" on win32) */
QString
VidaliaSettings::getTorApp()
{
  QString path = getTorPath();
#ifdef Q_OS_WIN32
  QString app = "tor.exe";
#else
  QString app = "tor";
#endif
  return (path + "/" + app);
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

