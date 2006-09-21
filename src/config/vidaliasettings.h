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
 * \file vidaliasettings.h
 * \version $Id$
 */

#ifndef _VIDALIASETTINGS_H
#define _VIDALIASETTINGS_H

#include <QHash>
#include <QSettings>
#include <control/logevent.h>


/** Handles saving and restoring Vidalia's settings, such as the
 * location of Tor, the control port, etc.
 *
 * NOTE: Qt 4.1 documentation states that constructing a QSettings object is
 * "very fast", so we shouldn't need to create a global instance of this
 * class.
 */
class VidaliaSettings : public QSettings
{
  
public:
  /** Default constructor. */
  VidaliaSettings();

  /** Resets all of Vidalia's settings. */
  static void reset();
  
  /** Sets the default value of <b>key</b> to be <b>val</b>. */
  void setDefault(QString key, QVariant val);
  /** Returns the default value for <b>key</b>. */
  QVariant defaultValue(QString key);
  /** Save <b>val</b> to the configuration file for the setting <b>key</b>, if
   * <b>val</b> is different than <b>key</b>'s current value. */
  void setValue(QString key, QVariant val);
  /** Returns the value for <b>key</b>. If no value is currently saved, then
   * the default value for <b>key</b> will be returned. */
  QVariant value(QString key);

  /** Gets the currently preferred language code for Vidalia. */
  QString getLanguageCode();
  /** Saves the preferred language code. */
  void setLanguageCode(QString languageCode);
 
  /** Gets the interface style key (e.g., "windows", "motif", etc.) */
  QString getInterfaceStyle();
  /** Sets the interface style key. */
  void setInterfaceStyle(QString styleKey);
  
  /** Returns true if Vidalia should start Tor when it starts. */
  bool runTorAtStart();
  /** Set whether to run Tor when Vidalia starts. */
  void setRunTorAtStart(bool run);

  /** Returns true if Vidalia should start on system boot. */
  bool runVidaliaOnBoot();
  /** Set whether to run Vidalia on system boot. */
  void setRunVidaliaOnBoot(bool run);

private:
  QHash<QString,QVariant> _defaults;
};

#endif

