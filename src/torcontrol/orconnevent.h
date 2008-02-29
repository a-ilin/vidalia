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
** \file orconnevent.h
** \version $Id$
** \brief Event dispatched upon receiving an ORCONN update from Tor
*/

#ifndef _ORCONNEVENT_H
#define _ORCONNEVENT_H

#include <QEvent>
#include <QString>

class OrConnEvent : public QEvent
{
public:
  /** OR connection status */
  enum Status {
    Unknown,   /**< Unknown OR status. */
    New,       /**< Incoming connection. */
    Launched,  /**< Outgoing connection. */
    Connected, /**< TLS handshake finished. */
    Failed,    /**< Connection closed before handshake completed. */
    Closed     /**< Connection closed that had previously handshaked. */
  };

  /** Contructor */
  OrConnEvent(Status status, QString server);

  /** Converts a string description of a connection's status to an enum value */
  static Status toStatus(QString status);
  
  /** Returns the status for this OR connection. */
  Status status();
  /** Returns the OR server with which this connection is associated. */
  QString server();
  
private:
  /** OR connection status associated with this event. */
  Status _status;
  /** OR server */
  QString _server;
};

#endif

