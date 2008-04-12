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
#include <QRegExp>

#include "circuit.h"


/** Default constructor. */
Circuit::Circuit()
{
  _circId  = 0;
  _status  = Unknown;
  _isValid = false;
}

/** Parses the string given in Tor control protocol format for a circuit. The
 * format is:
 * 
 *      CircuitID SP CircStatus [SP Path]
 *
 * If the status is "LAUNCHED", the Path is empty. Server names in the path
 * must follow Tor's VERBOSE_NAMES format.
 */
Circuit::Circuit(const QString &circuit)
{
  _isValid = false;

  QStringList parts = circuit.split(" ");
  if (parts.size() >= 2) {
    /* Get the circuit ID */
    _circId = (quint64)parts.at(0).toULongLong();
    /* Get the circuit status value */
    _status = Circuit::toStatus(parts.at(1));

    /* Get the circuit path (list of routers) */
    if (parts.size() > 2) {
      foreach (QString hop, parts.at(2).split(",")) {
        QStringList parts = hop.split(QRegExp("[=~]"));
        if (parts.size() != 2)
          return;

        _ids   << parts.at(0).mid(1);
        _names << parts.at(1);
      }
    }

    _isValid = true;
  }
}

/** Converts the circuit status string to its proper enum value */
Circuit::Status
Circuit::toStatus(const QString &status)
{
  if (!status.compare("LAUNCHED", Qt::CaseInsensitive))
    return Launched;
  if (!status.compare("BUILT", Qt::CaseInsensitive))
    return Built;
  if (!status.compare("EXTENDED", Qt::CaseInsensitive))
    return Extended;
  if (!status.compare("FAILED", Qt::CaseInsensitive))
    return Failed;
  if (!status.compare("CLOSED", Qt::CaseInsensitive))
    return Closed;
  return Unknown;
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

