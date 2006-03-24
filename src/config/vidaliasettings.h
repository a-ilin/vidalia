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

#include <QSettings>

#include <control/logevent.h>
#include <gui/bwgraph/linetypes.h>


/** Handles saving and restoring Vidalia's settings, such as the
 * location of Tor, the control port, etc.
 *
 * NOTE: Qt 4.1 documentation states that constructing a QSettings object is
 * "very fast", so we shouldn't need to create a global instance of this
 * class.
 */
class VidaliaSettings : protected QSettings
{
  
public:
  /** Default constructor. */
  VidaliaSettings();

  /** Return Vidalia's version string */
  static QString getVersion();

  /** Resets all of Vidalia's settings. */
  static void reset();

  /** Returns true if Vidalia should start Tor when it starts. */
  bool runTorAtStart();
  /** Set whether to run Tor when Vidalia starts. */
  void setRunTorAtStart(bool run);

  /** Returns true if Vidalia should start on system boot. */
  bool runVidaliaOnBoot();
  /** Set whether to run Vidalia on system boot. */
  void setRunVidaliaOnBoot(bool run);

  /** Get filter used to determine which messages to display is the
   * messagelog. */
  uint getMsgFilter();
  /** Set the filter used to determine which messages to display in the
   * message log. */
  void setMsgFilter(LogEvent::Severity severity, bool status);

  /** Get the maximum number of messages to display in log. */
  int getMaxMsgCount();
  /** Set the maximum number of messages to display in the message log. */
  void setMaxMsgCount(int max);

  /* Get the message log opacity setting. */
  int getMsgLogOpacity();
  /** Set the message log opacity setting.*/
  void setMsgLogOpacity(int value);

  /* Returns true if log messages are to be written to disk. */
  bool isLogFileEnabled();
  /** Enable or disable writing log messages to disk. */
  void enableLogFile(bool enable);
  
  /* Get the destination log file. */
  QString getLogFile();
  /** Set the destination log file. */
  void setLogFile(QString file);

  /* Get the bandwidth graph line filter. */
  uint getBWGraphFilter();
  /** Set the bandwidth graph line filter. */
  void setBWGraphFilter(uint line, bool status);

  /** Set the bandwidth graph opacity setting. */
  int getBWGraphOpacity();
  /** Set the bandwidth graph opacity settings. */
  void setBWGraphOpacity(int value);
};

#endif

