/****************************************************************
 *  $Id$
 * 
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

#ifndef _VIDALIASETTINGS_H
#define _VIDALIASETTINGS_H

#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QHostAddress>

/* On win32, we need to add the .exe onto Tor's filename */
#ifdef Q_OS_WIN32
#define TOR_EXECUTABLE    "tor.exe"
#else
#define TOR_EXECUTABLE    "tor"
#endif


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
  /* Default constructor and destructor */
  VidaliaSettings();
  ~VidaliaSettings();

  /* Return Vidalia's version string */
  static QString getVersion();
  
  /* Get and set Tor's location information */
  QDir        getTorPath();
  void        setTorPath(QDir path);
  QFileInfo   getTorExecutable();

  /* Add/remove/retrieve Tor command-line arguments */
  QStringList getTorArguments(); 
  void setTorArguments(QStringList args);
  void addTorArgument(QString arg);
  void removeTorArgument(QString arg);

  /* Get and set Tor's control address */
  QHostAddress getControlAddress();
  void setControlAddress(QHostAddress addr);

  /* Get and set Tor's control port */
  quint16 getControlPort();
  void    setControlPort(quint16 port);

  /* Get and set Tor's authentication information */
  QByteArray getAuthToken();
  void setAuthToken(QByteArray token);
};

#endif

