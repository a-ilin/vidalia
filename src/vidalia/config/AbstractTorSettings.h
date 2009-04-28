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
** \file AbstractTorSettings.h
** \version $Id$
** \brief Manages settings that need to be SETCONF'ed to Tor
*/

#ifndef _ABSTRACTTORSETTINGS_H
#define _ABSTRACTTORSETTINGS_H

#include "VSettings.h"
#include "TorControl.h"


class AbstractTorSettings : public VSettings
{
  Q_OBJECT

public:
  /** Constructor. All settings will be under the heading <b>group</b> and
   * <b>torControl</b> will be used to <i>getconf</i> values from Tor. */
  AbstractTorSettings(const QString &group, TorControl *torControl = 0);
  
  /** Sets a value indicating that the server settings have changed since
   * apply() was last called. */
  void setChanged(bool changed);
  /** Returns true if any settings have changed since the last time apply()
   * was called. */
  virtual bool changedSinceLastApply() const;
  /** Reverts all settings to their values at the last time apply() was
   * called. */
  virtual void revert();
  /** Subclasses must implement this to <i>setconf</i> values to apply them
   * to a running Tor instance. */
  virtual bool apply(QString *errmsg) = 0;

protected:
  /** If Vidalia is connected to Tor, this returns the value associated with
   * <b>key</b> by calling torValue(). Otherwise, this calls localValue()
   * to get the locally saved value associated with <b>key</b>. */
  virtual QVariant value(const QString &key) const;
  /** Returns the value associated with <b>key</b> saved in the local
   * configuration file. */
  virtual QVariant localValue(const QString &key) const;
  /** Returns the value associated with <b>key</b> by querying TOr via 
   * <i>getconf key</i>. */
  virtual QVariant torValue(const QString &key) const;
  /** Saves the value <b>val</b> for the setting <b>key</b> to the local
   * settings file. */
  virtual void setValue(const QString &key, const QVariant &value);
  
  /** Returns true if the given QVariant contains an empty value, depending on
   * the data type. For example, 0 is considered an empty integer and "" is
   * an empty string. */
  bool isEmptyValue(const QVariant &value) const;

protected:
  /** Returns the TorControl object used for reading settings from or writing
   * settings to Tor, if one was specified. Returns 0 if no TorControl object
   * was given. */
  TorControl* torControl() const { return _torControl; };

private:
  /** TorControl object used for reading settings from or applying settings to
   * Tor. */
  TorControl* _torControl;
  /** Collection of settings values at the last time apply() was called. */
  QMap<QString, QVariant> _backupSettings;
};

#endif

