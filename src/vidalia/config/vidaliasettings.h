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
 * \brief General Vidalia settings, such as language and interface style
 */

#ifndef _VIDALIASETTINGS_H
#define _VIDALIASETTINGS_H

#include "vsettings.h"


/** Handles saving and restoring Vidalia's settings, such as the
 * location of Tor, the control port, etc.
 *
 * NOTE: Qt 4.1 documentation states that constructing a QSettings object is
 * "very fast", so we shouldn't need to create a global instance of this
 * class.
 */
class VidaliaSettings : public VSettings
{
  Q_OBJECT

public:
  /** Default constructor. */
  VidaliaSettings();

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

  /** Returns true if Vidalia's main window should be visible when the
   * application starts. */
  bool showMainWindowAtStart();
  /** Sets whether to show Vidalia's main window when the application starts. */
  void setShowMainWindowAtStart(bool show);
  
  /** Returns true if Vidalia should start on system boot. */
  bool runVidaliaOnBoot();
  /** Set whether to run Vidalia on system boot. */
  void setRunVidaliaOnBoot(bool run);

  /** Returns a fully-qualified path to the web browser, including the
   * executable name. */
  QString getBrowserExecutable() const;
  /** Sets the location and name of the web browser executable to the given string.
   * If set to the empty string, the browser will not be started. */
  void setBrowserExecutable(const QString &browserExecutable);
};

#endif

