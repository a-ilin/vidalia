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
 * \file vsettings.cpp
 * \version $Id$
 * \brief Stores and retrieves settings from Vidalia's configuration file. 
 */

#include <vidalia.h>

#include "vsettings.h"

/** The file in which all settings will read and written. */
#define SETTINGS_FILE (Vidalia::dataDirectory() + "/vidalia.conf")


/** Constructor */
VSettings::VSettings(const QString settingsGroup)
: QSettings(SETTINGS_FILE, QSettings::IniFormat)
{
  if (!settingsGroup.isEmpty())
    beginGroup(settingsGroup);
}

/** Returns the saved value associated with <b>key</b>. If no value has been
 * set, the default value is returned.
 * \sa setDefault
 */
QVariant
VSettings::value(const QString &key, const QVariant &defaultVal)
{
  return QSettings::value(key, defaultVal.isNull() ? defaultValue(key)
                                                   : defaultVal);
}

/** Sets the value associated with <b>key</b> to <b>val</b>. */
void
VSettings::setValue(const QString &key, const QVariant &val)
{
  if (val == defaultValue(key))
    QSettings::remove(key);
  else if (val != value(key))
    QSettings::setValue(key, val);
}

/** Sets the default setting for <b>key</b> to <b>val</b>. */
void
VSettings::setDefault(const QString &key, const QVariant &val)
{
  _defaults.insert(key, val);
}

/** Returns the default setting value associated with <b>key</b>. If
 * <b>key</b> has no default value, then an empty QVariant is returned. */
QVariant
VSettings::defaultValue(const QString &key)
{
  if (_defaults.contains(key))
    return _defaults.value(key);
  return QVariant();
}

/** Resets all of Vidalia's settings. */
void
VSettings::reset()
{
  /* Static method, so we have to create a QSettings object. */
  QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
  settings.clear();
}

/** Returns a map of all currently saved settings at the last appyl() point. */
QMap<QString, QVariant>
VSettings::allSettings()
{
  QMap<QString, QVariant> settings;
  foreach (QString key, allKeys()) {
    settings.insert(key, value(key));
  }
  return settings;
}

