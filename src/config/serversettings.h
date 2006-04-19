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
 * \file serversettings.h
 * \version $Id$
 */

#ifndef _SERVERSETTINGS_H
#define _SERVERSETTINGS_H

#include <control/torcontrol.h>

#include "vidaliasettings.h"
#include "exitpolicy.h"


class ServerSettings : private VidaliaSettings
{

public:
  /** Constructor */
  ServerSettings(TorControl *torControl);

  /** Reverts all settings changes since the last apply. */
  void revert();
  /** Returns true if the settings have changed since the last apply. */
  bool changedSinceLastApply();
  /** Applies changese to Tor. */
  bool apply(QString *errmsg = 0);

  /** Enables running Tor as a server. */
  void setServerEnabled(bool enable);
  /** Returns true if Tor is running as a server. */
  bool isServerEnabled();

  /** Sets the server's ORPort value. */
  void setORPort(quint16 orPort);
  /** Gets the server's ORPort value. */
  quint16 getORPort();

  /** Sets the server's DirPort value. */
  void setDirPort(quint16 dirPort);
  /** Gets the server's DirPort value. */
  quint16 getDirPort();
 
  /** Sets the server's nickname. */
  void setNickname(QString nickname);
  /** Gets the server's nickname. */
  QString getNickname();
  
  /** Sets the server's external IP address. */
  void setAddress(QString address);
  /** Gets the server's external IP address. */
  QString getAddress();

  /** Sets the server operator's contact information. */
  void setContactInfo(QString info);
  /** Gets the server operator's contact information. */
  QString getContactInfo();

  /** Enables or disables the server to act as a directory mirror. */
  void setDirectoryMirror(bool mirror);
  /** Returns true if the server will mirror the directory. */
  bool isDirectoryMirror();
 
  /** Sets whether the server will act as a middleman or not. */
  void setMiddleman(bool middleman);
  /** Returns true if the server is a middleman server. */
  bool isMiddleman();

  /** Sets the exit policy for this server. */
  void setExitPolicy(ExitPolicy &policy);
  /** Gets the exit policy for this server. */
  ExitPolicy getExitPolicy();

  /** Sets whether we automatically update the server address or not. */
  void setAutoUpdateAddress(bool enabled);
  /** Gets whether we automatically update the server address. */
  bool getAutoUpdateAddress();

private:
  /** Sets a value indicating that the server settings have changed since
   * apply() was last called. */
  void setChanged(bool changed);
  
  /** Returns Tor-recognizable configuration keys and current values. */
  QHash<QString,QString> confValues();

  /** Returns all currently stored server settings. */
  QMap<QString, QVariant> allSettings();
  
  /** Returns true if the specified QVariant contains an empty value. */
  bool isEmptyValue(QVariant value);
  /** Retrieves a configuration value. If one isn't found, use a default. */
  QVariant value(QString key, QVariant defaultValue);
  /** Stores a configuration key-value. */
  void setValue(QString key, QVariant value);

  /** A TorControl object used to talk to Tor. */
  TorControl* _torControl;
  /** Values of all stored settings at the last apply() point. */
  QMap<QString, QVariant> _backupSettings;
};

#endif

