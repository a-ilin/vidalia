/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file AbstractTorSettings.cpp
** \brief Manages settings that need to be SETCONF'ed to Tor
*/

#include "AbstractTorSettings.h"

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

