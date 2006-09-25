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
 * \file circuit.cpp
 * \version $Id$
 * \brief Object representing a Tor circuit
 */

#include <QStringList>

#include "circuit.h"


/** Default constructor. */
Circuit::Circuit()
{
  _circId = 0;
  _status = Unknown;
  _path   = QString();
}

/** Constructor */
Circuit::Circuit(quint64 circId, Status status, QString path)
{
  _circId = circId;
  _status = status;
  _path   = path;
}

/** Parses the string given in Tor control protocol format for a circuit. The
 * format is:
 * 
 *      CircuitID SP CircStatus [SP Path]
 *
 * If the status is "LAUNCHED", the Path is empty.
 */
Circuit
Circuit::fromString(QString circuit)
{
  QStringList parts = circuit.split(" ");
  if (parts.size() >= 2) {
    /* Get the circuit ID */
    quint64 circId = (quint64)parts.at(0).toULongLong();
    /* Get the circuit status value */
    Circuit::Status status = Circuit::toStatus(parts.at(1));
    /* Get the circuit path (list of routers) */
    QString path = (parts.size() > 2 ? parts.at(2) : "");

    return Circuit(circId, status, path);
  }
  return Circuit();
}

/** Converts the circuit status string to its proper enum value */
Circuit::Status
Circuit::toStatus(QString strStatus)
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

/** Returns a string representation of the circuit's status. */
QString
Circuit::statusString()
{
  QString status;
  switch (_status) {
    case Launched:    status = tr("New"); break;
    case Built:       status = tr("Open"); break;
    case Extended:    status = tr("Building"); break;
    case Failed:      status = tr("Failed");  break;
    case Closed:      status = tr("Closed"); break;
    default:          status = tr("Unknown"); break;
  }
  return status;
}

/** Returns true if all fields in this Circuit object are empty. */
bool
Circuit::isEmpty()
{
  return (!_circId && (_status == Unknown));
}

