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
 * \brief Settings for running a Tor server
 */

#ifndef _SERVERSETTINGS_H
#define _SERVERSETTINGS_H

#include "abstracttorsettings.h"
#include "exitpolicy.h"


class ServerSettings : public AbstractTorSettings
{
  Q_OBJECT

public:
  /** Constructor */
  ServerSettings(TorControl *torControl);

  /** Applies changese to Tor. */
  bool apply(QString *errmsg = 0);

  /** Enables running Tor as a server. */
  void setServerEnabled(bool enable);
  /** Returns true if Tor is running as a server. */
  bool isServerEnabled();
  
  /** Sets to <b>enabled</b> whether Tor should be a bridge node when acting
   * as a server. */
  void setBridgeEnabled(bool enable);
  /** Returns true if Tor is configured to act as a bridge node. */
  bool isBridgeEnabled();

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
  
  /** Sets the server operator's contact information. */
  void setContactInfo(QString info);
  /** Gets the server operator's contact information. */
  QString getContactInfo();

  /** Enables or disables the server to act as a directory mirror. */
  void setDirectoryMirror(bool mirror);
  /** Returns true if the server will mirror the directory. */
  bool isDirectoryMirror();
  /** Sets the exit policy for this server. */
  void setExitPolicy(ExitPolicy &policy);
  /** Gets the exit policy for this server. */
  ExitPolicy getExitPolicy();
  
  /** Sets the long-term average bandwidth rate (in KB/s) of this server. */
  void setBandwidthAvgRate(quint32 rate);
  /** Gets the long-term average bandwidth rate (in KB/s) of this server. */
  quint32 getBandwidthAvgRate();

  /** Sets the maximum burst rate (in B/s) of this server. */
  void setBandwidthBurstRate(quint32 rate);
  /** Gets the maximum burst rate (in B/s) of this server. */
  quint32 getBandwidthBurstRate();

protected:
  /** Virtual method called when we retrieve a server-related setting from Tor.
   * Currently this just translates BandwidthFoo to RelayBandwidthFoo when
   * appropriate. */
  virtual QVariant torValue(const QString &key) const;

private:
  /** Returns Tor-recognizable configuration keys and current values. */
  QHash<QString,QString> confValues();
};

#endif

