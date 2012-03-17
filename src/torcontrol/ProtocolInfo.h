/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file ProtocolInfo.h
** \brief Container for information in a PROTOCOLINFO reply from Tor
*/

#ifndef _PROTOCOLINFO_H
#define _PROTOCOLINFO_H

#include <QStringList>


class ProtocolInfo
{
public:
  /** Default constructor. */
  ProtocolInfo() {}

  /** Returns true if this ProtocolInfo object contains no data. */
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

