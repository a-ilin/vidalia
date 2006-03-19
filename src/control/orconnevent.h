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
 * \file orconnevent.h
 * \version $Id$
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

