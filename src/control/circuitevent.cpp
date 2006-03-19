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
 * \file circuitevent.cpp
 * \version $Id$
 */

#include "eventtype.h"
#include "circuitevent.h"

/** Default constructor */
CircuitEvent::CircuitEvent(quint64 circId, Status status, QString path)
: QEvent((QEvent::Type)CustomEventType::CircuitEvent)
{
  _circId = circId;
  _status = status;
  _path   = path;
}

/** Converts the circuit status string to its proper enum value */
CircuitEvent::Status
CircuitEvent::toStatus(QString strStatus)
{
  Status status;
  strStatus = strStatus.toUpper();
  if (strStatus == "LAUNCHED") {
    status = Launched;
  } else if (strStatus == "BUILT") {
    status = Built;
  } else if (strStatus == "EXTENDED") {
    status = Extended;
  } else if (strStatus == "FAILED") {
    status = Failed;
  } else if (strStatus == "CLOSED") {
    status = Closed;
  } else {
    status = Unknown;
  }
  return status;
}

/** Returns the ID assigned to this circuit */
quint64
CircuitEvent::circuitId()
{
  return _circId;
}

/** Returns the new status for this circuit */
CircuitEvent::Status
CircuitEvent::status()
{
  return _status;
}

/** Returns the path used for this circuit */
QString
CircuitEvent::path()
{
  return _path;
}
