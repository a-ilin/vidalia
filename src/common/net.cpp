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
** \file net.cpp
** \version $Id$
** \brief Common network I/O and utility functions
*/

#include "net.h"

#include <QTcpSocket>


/** Attempts a connection to <b>host</b> on <b>port</b>. Returns true if the
 * connection was successful, or false if the connection attempt failed. */
bool
net_test_connect(QHostAddress host, quint16 port, int timeout)
{
  QTcpSocket sock;
  sock.connectToHost(host, port);
  if (!sock.waitForConnected(timeout)) {
    return false;
  }
  sock.disconnectFromHost();
  return true;
}

