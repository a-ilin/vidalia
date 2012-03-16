/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file VSettings.cpp
** \brief Stores and retrieves settings from Vidalia's configuration file.
*/

#include "VSettings.h"
#include "Vidalia.h"

#include <QFileInfo>

/** The file in which all settings will read and written. */
#define SETTINGS_FILE (Vidalia::dataDirectory() + "/vidalia.conf")
#define SETTINGS_DEFAULTS_FILE (Vidalia::dataDirectory() + "/vidalia.defaults.conf")


/** Constructor */
VSettings::VSettings(const QString settingsGroup)
: QSettings(SETTINGS_FILE, QSettings::IniFormat)
{
  _defaultSettings = new QSettings(SETTINGS_DEFAULTS_FILE,
                                   QSettings::IniFormat);

  if (!settingsGroup.isEmpty())
    beginGroup(settingsGroup);
}

VSettings::~VSettings()
{
  delete _defaultSettings;
}

/** Returns the location of Vidalia's configuration settings file. */
QString
VSettings::settingsFile()
{
  return SETTINGS_FILE;
}

/** Returns true if Vidalia's configuration settings file already exists. */
bool
VSettings::settingsFileExists()
{
  QFileInfo fi(settingsFile());
  return fi.exists();
}

/** Returns the saved value associated with <b>key</b>. If no value has been
 * set, the default value is returned.
 * \sa setDefault
 */
QVariant
VSettings::value(const QString &key, const QVariant &defaultVal) const
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
  _defaults.insert(key, _defaultSettings->value(key, val));
}

/** Returns the default setting value associated with <b>key</b>. If
 * <b>key</b> has no default value, then an empty QVariant is returned. */
QVariant
VSettings::defaultValue(const QString &key) const
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
VSettings::allSettings() const
{
  QMap<QString, QVariant> settings;
  foreach (QString key, allKeys()) {
    settings.insert(key, value(key));
  }
  return settings;
}

QVariant
VSettings::volatileValue(const QString &key,
                         const QVariant &defaultVal) const
{
  if(_volatileSettings.count(key) == 0)
    return defaultVal;
  return _volatileSettings.value(key);
}

void
VSettings::setVolatileValue(const QString &key, const QVariant &val)
{
  _volatileSettings.insert(key, val);
}
