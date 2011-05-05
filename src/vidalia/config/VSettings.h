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
** \file vsettings.h
** \brief Stores and retrieves settings from Vidalia's configuration file.
*/

#ifndef _VSETTINGS_H
#define _VSETTINGS_H

#include <QHash>
#include <QSettings>


class VSettings : public QSettings
{
  Q_OBJECT

public:
  /** Default constructor. The optional parameter <b>group</b> can be used to
   * set a prefix that will be prepended to keys specified to VSettings in
   * value() and setValue(). */
  VSettings(const QString group = QString());

  /** Returns the location of Vidalia's configuration settings file. */
  static QString settingsFile();
  /** Returns true if Vidalia's configuration settings file already exists. */
  static bool settingsFileExists();

  /** Resets all of Vidalia's settings. */
  static void reset();

  /** Returns the saved value associated with <b>key</b>. If no value has been
   * set, the default value is returned.
   * \sa setDefault
   */
  virtual QVariant value(const QString &key,
                         const QVariant &defaultVal = QVariant()) const;
  /** Sets the value associated with <b>key</b> to <b>val</b>. */
  virtual void setValue(const QString &key, const QVariant &val);

protected:
  /** Sets the default setting for <b>key</b> to <b>val</b>. */
  void setDefault(const QString &key, const QVariant &val);
  /** Returns the default setting value associated with <b>key</b>. If
   * <b>key</b> has no default value, then an empty QVariant is returned. */
  QVariant defaultValue(const QString &key) const;
  /** Returns a map of all currently saved settings at the last apply()
   * point. */
  QMap<QString, QVariant> allSettings() const;

private:
  /** Association of setting key names to default setting values. */
  QHash<QString, QVariant> _defaults; 
};

#endif

