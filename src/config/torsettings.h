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
 * \file torsettings.h
 * \version $Id$
 */

#ifndef _TORSETTINGS_H
#define _TORSETTINGS_H

#include <QHostAddress>

#include "vidaliasettings.h"

/** Manages Tor-specific settings, such as location, command-line arguments,
 * and control interface information. */
class TorSettings : private VidaliaSettings
{
public:
  /** Default constructor. */
  TorSettings();
  
  /** Gets the path to Tor's executable. */
  QString  getPath();
  /** Sets the path to Tor's executable. */
  void setPath(QString path);

  /** Gets the name of Tor's executable. */
  QString getExecutable();
 
  /** Builds and formats a list of command-line arguments. */
  QString getArguments();
  
  /** Gets the torrc to use when starting Tor. */
  QString getTorrc();
  /** Sets the torrc to use when starting Tor. */
  void setTorrc(QString torrc);
  /** Returns true if Tor uses an alternate torrc. */
  bool usingAlternateTorrc();
  
  /** Get Tor's control interface address. */
  QHostAddress getControlAddress();
  /** Set Tor's control interface address. */
  void setControlAddress(QHostAddress addr);
  
  /** Get the control port. */
  quint16 getControlPort();
  /** Set the control port. */
  void setControlPort(quint16 port);
  
  /** Get Tor's controller authentication information. */
  QByteArray getAuthToken();
  /** Set Tor's controller authentication information. */
  void setAuthToken(QByteArray token);
  
  /** Get which user will be used to run Tor. */
  QString getUser();
  /** Set which user will be used to run Tor. */
  void setUser(QString user);
  
  /** Get which group will be used to run Tor. */
  QString getGroup();
  /** Set which group will be used to run Tor. */
  void setGroup(QString group);

private:
  /** Returns a QMap of all currently set command-line arguments. */
  QMap<QString, QVariant> getArgumentsMap(); 
  /** Returns true if the given argument is set. */
  bool hasArgument(QString arg);
  /** Sets Tor's command-line arguments to the given list. */
  void setArguments(QMap<QString, QVariant> args);
  /** Sets a specific command-line argument to the given value. */
  void setArgument(QString arg, QString value);
  /** Unsets the given argument, removing it from Tor's command-line. */
  void unsetArgument(QString arg);
  /** Returns the value of the given command-line argument, or defaultValue is
   * no value is set for the argument. */
  QVariant getArgument(QString arg, QVariant defaultValue);
};

#endif

