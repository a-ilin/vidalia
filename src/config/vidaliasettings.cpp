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
 * \file vidaliasettings.cpp
 * \version $Id$
 * \brief General Vidalia settings, such as language and interface style
 */

#include <QDir>
#include <QCoreApplication>
#include <lang/languagesupport.h>
#include <vidalia.h>

#include "vidaliasettings.h"

#if defined(Q_WS_WIN)
#include <util/win32.h>
#endif


#define SETTING_LANGUAGE            "LanguageCode"
#define SETTING_STYLE               "InterfaceStyle"
#define SETTING_RUN_TOR_AT_START    "RunTorAtStart"
#define SETTING_DATA_DIRECTORY      "DataDirectory"

/* Default Vidalia Settings */
#if defined(Q_WS_MAC)
#define DEFAULT_STYLE               "macintosh (aqua)"
#else
#define DEFAULT_STYLE               "plastique"
#endif

#if defined(Q_OS_WIN32)
#define STARTUP_REG_KEY        "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define VIDALIA_REG_KEY        "Vidalia" 
#endif

/** The location of Vidalia's settings and configuration file. */
#define SETTINGS_FILE   (Vidalia::dataDirectory() + "/vidalia.conf")


/** Default Constructor */
VidaliaSettings::VidaliaSettings()
: QSettings(SETTINGS_FILE, QSettings::IniFormat)
{
  setDefault(SETTING_LANGUAGE, LanguageSupport::defaultLanguageCode());
  setDefault(SETTING_RUN_TOR_AT_START, true);
  setDefault(SETTING_STYLE, DEFAULT_STYLE);
}

/** Sets the default value of <b>key</b> to be <b>val</b>. */
void
VidaliaSettings::setDefault(QString key, QVariant val)
{
  _defaults.insert(key, val);
}

/** Returns the default value for <b>key</b>. */
QVariant
VidaliaSettings::defaultValue(QString key)
{
  if (_defaults.contains(key)) {
    return _defaults.value(key);
  }
  return QVariant();
}

/** Save <b>val</b> to the configuration file for the setting <b>key</b>, if
 * <b>val</b> is different than <b>key</b>'s current value. */
void
VidaliaSettings::setValue(QString key, QVariant val)
{
  if (value(key) != val) {
    QSettings::setValue(key, val);
  }
}

/** Returns the value for <b>key</b>. If no value is currently saved, then the
 * default value for <b>key</b> will be returned. */
QVariant
VidaliaSettings::value(QString key)
{
  return value(key, defaultValue(key));
}

/** Returns the value for <b>key</b>. If no value is currently saved, then
 * <b>defaultValue</b> will be returned. */
QVariant
VidaliaSettings::value(QString key, QVariant defaultValue)
{
  return QSettings::value(key, defaultValue);
}

/** Resets all of Vidalia's settings. */
void
VidaliaSettings::reset()
{
  QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
  settings.clear();
}

/** Gets the currently preferred language code for Vidalia. */
QString
VidaliaSettings::getLanguageCode()
{
  return value(SETTING_LANGUAGE).toString();
}

/** Sets the preferred language code. */
void
VidaliaSettings::setLanguageCode(QString languageCode)
{
  setValue(SETTING_LANGUAGE, languageCode);
}

/** Gets the interface style key (e.g., "windows", "motif", etc.) */
QString
VidaliaSettings::getInterfaceStyle()
{
  return value(SETTING_STYLE).toString();
}

/** Sets the interface style key. */
void
VidaliaSettings::setInterfaceStyle(QString styleKey)
{
  setValue(SETTING_STYLE, styleKey);
}

/** Returns true if Tor is to be run when Vidalia starts. */
bool
VidaliaSettings::runTorAtStart()
{
  return value(SETTING_RUN_TOR_AT_START).toBool();
}

/** If <b>run</b> is set to true, then Tor will be run when Vidalia starts. */
void
VidaliaSettings::setRunTorAtStart(bool run)
{
  setValue(SETTING_RUN_TOR_AT_START, run);
}

/** Returns true if Vidalia is set to run on system boot. */
bool
VidaliaSettings::runVidaliaOnBoot()
{
#if defined(Q_WS_WIN)
  if (!win32_registry_get_key_value(STARTUP_REG_KEY, VIDALIA_REG_KEY).isEmpty()) {
    return true;
  } else {
    return false;
  }
#else
  /* Platforms other than windows aren't supported yet */
  return false;
#endif
}

/** If <b>run</b> is set to true, then Vidalia will run on system boot. */
void
VidaliaSettings::setRunVidaliaOnBoot(bool run)
{
#if defined(Q_WS_WIN)
  if (run) {
    win32_registry_set_key_value(STARTUP_REG_KEY, VIDALIA_REG_KEY,
        QString("\"" +
                QDir::convertSeparators(QCoreApplication::applicationFilePath())) +
                "\"");
  } else {
    win32_registry_remove_key(STARTUP_REG_KEY, VIDALIA_REG_KEY);
  }
#else
  /* Platforms othe rthan windows aren't supported yet */
  Q_UNUSED(run);
  return;
#endif
}

