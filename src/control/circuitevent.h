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
 * \file circuitevent.h
 * \version $Id$
 */

#ifndef _CIRCUITEVENT_H
#define _CIRCUITEVENT_H

#include <QString>
#include <QEvent>


class CircuitEvent : public QEvent
{
public:
  /** Circuit status events */
  enum Status {
    Unknown,   /**< Unknown circuit status */
    Launched,  /**< Circuit ID assigned to new circuit */
    Built,     /**< All hops finished */
    Extended,  /**< Circuit extended by one hop */
    Failed,    /**< Circuit closed (was not built) */
    Closed     /**< Circuit closed (was built) */
  };
  
  /** Default constructor */
  CircuitEvent(quint64 circId, Status status, QString path);
  
  /** Converts a string description of a circuit's status to an enum value */
  static Status toStatus(QString strStatus);
  
  /** Returns the ID for this circuit */
  quint64 circuitId();
  /** Returns the status of this circuit */
  Status status();
  /** Returns the path chosen for this circuit */
  QString path();
  
private:
  /** Circuit ID */
  quint64 _circId;
  /** Circuit status */
  Status _status;
  /** Circuit path */
  QString _path;
};

#endif
