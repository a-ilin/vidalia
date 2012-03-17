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
** \file ProtocolInfo.cpp
** \brief Container for information in a PROTOCOLINFO reply from Tor
*/

#include "ProtocolInfo.h"


/** Returns true if this ProtocolInfo object contains no data. */
bool
ProtocolInfo::isEmpty() const
{
  return (_torVersion.isEmpty()
            && _authMethods.isEmpty()
            && _cookieAuthFile.isEmpty());
}

/** Sets the authentication methods Tor currently accepts. <b>methods</b>
 * should be a comma-delimited list of authentication methods. */
void
ProtocolInfo::setAuthMethods(const QString authMethods)
{
  _authMethods = authMethods.split(",");
}

