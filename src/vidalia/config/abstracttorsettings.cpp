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
 * \file abstracttorsettings.cpp
 * \version $Id$
 * \brief Manages settings that need to be SETCONF'ed to Tor
 */

#include "abstracttorsettings.h"

/** Setting that gets set to <i>true</i> if any settings in the current
 * settings group have been changed since the last time apply() was called. */
#define SETTING_CHANGED     "Changed"


/** Constructor. All settings will be under the heading <b>group</b> and
 * <b>torControl</b> will be used to <i>getconf</i> values from Tor. */
AbstractTorSettings::AbstractTorSettings(const QString &group,
                                         TorControl *torControl)
: VSettings(group)
{
  _torControl = torControl;
  _backupSettings = allSettings();

  setDefault(SETTING_CHANGED, false);
}

/** Reverts all settings to their values at the last time apply() was
 * called. */
void
AbstractTorSettings::revert()
{
  remove("");
  foreach (QString key, _backupSettings.keys()) {
    setValue(key, _backupSettings.value(key));
  }
}

/** Returns true if any settings have changed since the last time apply()
 * was called. */
bool
AbstractTorSettings::changedSinceLastApply() const
{
  return localValue(SETTING_CHANGED).toBool();
}

/** Sets a value indicating that the server settings have changed since
 * apply() was last called. */
void
AbstractTorSettings::setChanged(bool changed)
{
  VSettings::setValue(SETTING_CHANGED, changed);
  if (!changed)
    _backupSettings = allSettings();
}

/** Returns true if the given QVariant contains an empty value, depending on
 * the data type. For example, 0 is considered an empty integer and "" is
 * an empty string. */
bool
AbstractTorSettings::isEmptyValue(const QVariant &value) const
{
  switch (value.type()) {
    case QVariant::String: 
      return (value.toString().isEmpty());
    case QVariant::StringList:
      return (value.toStringList().isEmpty());
    case QVariant::UInt:
    case QVariant::Int:
      return (value.toUInt() == 0); 
    case QVariant::Invalid:
      return true;
    default:  break;
  }
  return false;
}

/** Returns the value associated with <b>key</b> saved in the local
 * configuration file. */
QVariant
AbstractTorSettings::localValue(const QString &key) const
{
  return VSettings::value(key);
}

/** Returns the value associated with <b>key</b> by querying TOr via 
 * <i>getconf key</i>. */
QVariant
AbstractTorSettings::torValue(const QString &key) const
{
  QVariant defaultVal;
  QVariant confValue;

  defaultVal = defaultValue(key);
  if (_torControl) {
    confValue = _torControl->getConf(key);
    confValue.convert(defaultVal.type());
  }
  return (isEmptyValue(confValue) ? localValue(key) : confValue);
}

/** If Vidalia is connected to Tor, this returns the value associated with
 * <b>key</b> by calling torValue(). Otherwise, this calls localValue() to get
 * the locally saved value associated with <b>key</b>. */
QVariant
AbstractTorSettings::value(const QString &key) const
{
  if (_torControl && _torControl->isConnected() && !changedSinceLastApply())
    return torValue(key);
  return localValue(key);
}

/** Saves the value <b>val</b> for the setting <b>key</b> to the local
 * settings file. */
void
AbstractTorSettings::setValue(const QString &key, const QVariant &value)
{
  if (value != localValue(key)) {
    setChanged(true);
    VSettings::setValue(key, value);
  }
}

