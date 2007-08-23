/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
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
 * \file protocolinfo.h
 * \version $Id$
 * \brief Container for information in a PROTOCOLINFO reply from Tor
 */

#ifndef _PROTOCOLINFO_H
#define _PROTOCOLINFO_H

#include <QStringList>


class ProtocolInfo
{
public:
  /** Default constructor. */
  ProtocolInfo() {}
  
  /** Returns true if this ProtoclInfo object contains no data. */
  bool isEmpty() const;
  
  /** Sets the authentication methods Tor currently accepts. <b>methods</b>
   * should be a comma-delimited list of authentication methods. */
  void setAuthMethods(const QString methods);
  /** Returns the authentication methods Tor currently accepts. */
  QStringList authMethods() const { return _authMethods; }

  /** Sets the file to which Tor has written its authentication cookie. */
  void setCookieAuthFile(const QString cookieAuthFile)
    { _cookieAuthFile = cookieAuthFile; }
  /** Returns the file to which Tor has written its authentication cookie. */
  QString cookieAuthFile() const { return _cookieAuthFile; }

  /** Sets the version of Tor to which the controller is connected. */
  void setTorVersion(const QString torVersion) { _torVersion = torVersion; }
  /** Returns the version of Tor to which the controller is connected. */
  QString torVersionString() const { return _torVersion; }

private:
  QString _torVersion;      /**< The Tor version in the PROTOCOLINFO reply. */
  QString _cookieAuthFile;  /**< Tor's authentication cookie file. */
  QStringList _authMethods; /**< Tor's ccepted authentication methods. */
};

#endif

