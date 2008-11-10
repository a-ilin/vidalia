/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file torsettings.h
** \version $Id$
** \brief Settings used for starting and running Tor
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

