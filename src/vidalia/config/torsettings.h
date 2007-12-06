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
 * \file torsettings.h
 * \version $Id$
 * \brief Settings used for starting and running Tor
 */

#ifndef _TORSETTINGS_H
#define _TORSETTINGS_H

#include <QHostAddress>

#include "abstracttorsettings.h"


/** Manages Tor-specific settings, such as location, command-line arguments,
 * and control interface information. */
class TorSettings : public AbstractTorSettings
{
  Q_OBJECT

public:
  /** Available Tor authentication methods. */
  enum AuthenticationMethod {
    NullAuth,      /**< No authentication. */
    CookieAuth,    /**< Use a "magic" cookie for authentication. */
    PasswordAuth,  /**< Use a hashed password for authentication. */
    UnknownAuth    /**< Unknown authentication method. */
  };
  
  /** Default constructor. */
  TorSettings(TorControl *torControl = 0);
  /** Applies any changes to Tor's control port or authentication settings. */ 
  bool apply(QString *errmsg = 0);

  /** Gets the name and path of Tor's executable. */
  QString getExecutable() const;
  /** Sets the name and path of Tor's executable. */
  void setExecutable(const QString &torExecutable);
 
  /** Gets the location of Tor's data directory. */
  QString getDataDirectory() const;
  /** Sets the location to use for Tor's data directory. */
  void setDataDirectory(const QString &dataDir);
  
  /** Gets the torrc to use when starting Tor. */
  QString getTorrc() const;
  /** Sets the torrc to use when starting Tor. */
  void setTorrc(const QString &torrc);
  
  /** Get Tor's control interface address. */
  QHostAddress getControlAddress() const;
  /** Set Tor's control interface address. */
  void setControlAddress(const QHostAddress &addr);
  
  /** Get the control port. */
  quint16 getControlPort() const;
  /** Set the control port. */
  void setControlPort(quint16 port);
  
  /** Returns the plaintext (i.e., not hashed) control password used when
   * authenticating to Tor. */
  QString getControlPassword() const;
  /** Sets the control password used when starting Tor with
   * HashedControlPassword to <b>password</b>. */
  void setControlPassword(const QString &password);

 /** Returns true if a new, random control password is to be used each time
  * Tor is started. */
  bool useRandomPassword() const;
  /** Sets whether or not to generate and use a random control password each
   * time Tor is started. */
  void setUseRandomPassword(bool useRandomPassword);

  /** Returns the current authentication method used when connecting to Tor.*/
  AuthenticationMethod getAuthenticationMethod() const;
  /** Sets the authentication method used when starting Tor to <b>method</b>.*/
  void setAuthenticationMethod(AuthenticationMethod method);
   
  /** Get which user will be used to run Tor. */
  QString getUser() const;
  /** Set which user will be used to run Tor. */
  void setUser(const QString &user);
  
  /** Get which group will be used to run Tor. */
  QString getGroup() const;
  /** Set which group will be used to run Tor. */
  void setGroup(const QString &group);

  /** Generates a random control password consisting of PASSWORD_LEN
   * characters. */
  static QString randomPassword();
  /** Returns the hash of <b>password</b> as given by the command 
   * "tor --hash-password foo". */
  static QString hashPassword(const QString &password);

private:
  /** Returns the AuthenticationMethod enum value for the string
   * description of the authentication method given in <b>authMethod</b>. */
  AuthenticationMethod toAuthenticationMethod(const QString &authMethod) const;
  /** Returns the string description of the authentication method specified by
   * <b>method</b>. The authentication method string is stored in  Vidalia's
   * configuration file. */
  QString toString(AuthenticationMethod type) const;
};

#endif

