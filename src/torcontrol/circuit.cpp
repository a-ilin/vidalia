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
** \file circuit.cpp
** \version $Id$
** \brief Object representing a Tor circuit
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

/** Constructor */
Circuit::Circuit(quint64 circId, Status status, QStringList hops)
{
  _circId = circId;
  _status = status;
  _path   = hops.join(",");
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
Circuit::statusString() const
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
Circuit::isEmpty() const
{
  return (!_circId && (_status == Unknown));
}

