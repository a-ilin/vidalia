/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to
**  the terms described in the LICENSE file.
*/

/*
** \file OrConnEvent.cpp
** \version $Id$
** \brief Event dispatched upon receiving an ORCONN update from Tor
*/

#include "OrConnEvent.h"
#include "eventtype.h"


/** Constructor
 * \param status OR connection status.
 * \param server OR server.
 */
OrConnEvent::OrConnEvent(Status status, QString server)
: QEvent((QEvent::Type)CustomEventType::OrConnEvent)
{
  _status   = status;
  _server = server;
}

/** Converts a string description of a connection's status to an enum value */
OrConnEvent::Status
OrConnEvent::toStatus(QString status)
{
  Status s;
  status = status.toUpper();
  if (status == "NEW") {
    s = New;
  } else if (status == "LAUNCHED") {
    s = Launched;
  } else if (status == "CONNECTED") {
    s = Connected;
  } else if (status == "FAILED") {
    s = Failed;
  } else if (status == "CLOSED") {
    s = Closed;
  } else {
    s = Unknown;
  }
  return s;
}

/** Returns the status for this OR connection. */
OrConnEvent::Status
OrConnEvent::status()
{
  return _status;
}

/** Returns the OR server with which this connection is associated. */
QString
OrConnEvent::server()
{
  return _server;
}

