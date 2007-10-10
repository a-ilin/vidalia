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
 * \file vsettings.h
 * \version $Id$
 * \brief Stores and retrieves settings from Vidalia's configuration file.
 */

#ifndef _VSETTINGS_H
#define _VSETTINGS_H

#include <QHash>
#include <QSettings>


class VSettings : public QSettings
{
public:
  /** Default constructor. The optional parameter <b>group</b> can be used to
   * set a prefix that will be prepended to keys specified to VSettings in
   * value() and setValue(). */
  VSettings(const QString group = QString());

  /** Resets all of Vidalia's settings. */
  static void reset();

  /** Returns the saved value associated with <b>key</b>. If no value has been
   * set, the default value is returned.
   * \sa setDefault
   */
  virtual QVariant value(const QString &key,
                         const QVariant &defaultVal = QVariant());
  /** Sets the value associated with <b>key</b> to <b>val</b>. */
  virtual void setValue(const QString &key, const QVariant &val);

protected:
  /** Sets the default setting for <b>key</b> to <b>val</b>. */
  void setDefault(const QString &key, const QVariant &val);
  /** Returns the default setting value associated with <b>key</b>. If
   * <b>key</b> has no default value, then an empty QVariant is returned. */
  QVariant defaultValue(const QString &key);
  /** Returns a map of all currently saved settings at the last apply()
   * point. */
  QMap<QString, QVariant> allSettings();

private:
  /** Association of setting key names to default setting values. */
  QHash<QString, QVariant> _defaults; 
};

#endif

