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
** \file Circuit.cpp
** \brief Object representing a Tor circuit
*/

#include "tcglobal.h"
#include "Circuit.h"
#include "stringutil.h"

#include <QStringList>
#include <QRegExp>
#include <QDebug>


/** Default constructor. */
Circuit::Circuit()
  : _status(Unknown),
    _isValid(false),
    _buildFlags(None),
    _purpose(General)
{}

/** Parses the string given in Tor control protocol format for a circuit. The
 * format is:
 *
 *    CircuitID SP CircStatus [SP Path]
 *    [SP "BUILD_FLAGS=" BuildFlags] [SP "PURPOSE=" Purpose]
 *    [SP "HS_STATE=" HSState] [SP "REND_QUERY=" HSAddress]
 *    [SP "TIME_CREATED=" TimeCreated]
 *    [SP "REASON=" Reason [SP "REMOTE_REASON=" Reason]] CRLF
 *
 *
 * If the status is "LAUNCHED", the Path is empty. Server names in the path
 * must follow Tor's VERBOSE_NAMES format.
 */
Circuit::Circuit(const QString &circuit)
{
  QStringList parts = circuit.split(" ", QString::SkipEmptyParts);
  if (parts.size() >= 2) {
    /* Get the circuit ID */
    _circId = parts.at(0);
    if (! isValidCircuitId(_circId))
      goto err;

    /* Get the circuit status value */
    _status = Circuit::toStatus(parts.at(1));

    /* Get the circuit path (list of routers) */
    if (parts.size() > 2 && parts.at(2).startsWith("$")) {
      foreach (QString hop, parts.at(2).split(",")) {
        QStringList parts = hop.split(QRegExp("[=~]"));
        if (parts.size() != 2)
          goto err;

        _ids   << parts.at(0).mid(1);
        _names << parts.at(1);
      }
    }

    if(parts.size() > 3) {
      parts.removeFirst(); // remove id
      parts.removeFirst(); // remove status
      parts.removeFirst(); // remove path

      bool ok;
      QHash<QString,QString> rest = string_parse_keyvals(parts.join(" "), &ok);
      foreach(const QString key, rest.keys()) {
        if(key.compare("BUILD_FLAGS", Qt::CaseInsensitive) == 0) {
          _buildFlags = None;
          foreach(const QString flag, rest.value(key).split(","))
             _buildFlags |= flagValue(flag);
        } else if(key.compare("PURPOSE", Qt::CaseInsensitive) == 0) {
          _purpose = purposeValue(rest.value(key));
        }
      }
    }

    _isValid = true;
  }
  return;

err:
  tc::warn("Improperly formatted circuit: '%1'").arg(circuit);
  _isValid = false;
}

Circuit::BuildFlag
Circuit::flagValue(const QString &flag) const
{
  if (!flag.compare("ONEHOP_TUNNEL", Qt::CaseInsensitive))
    return OneHopTunnel;
  if (!flag.compare("IS_INTERNAL", Qt::CaseInsensitive))
    return IsInternal;
  if (!flag.compare("NEED_CAPACITY", Qt::CaseInsensitive))
    return NeedCapacity;
  if (!flag.compare("NEED_UPTIME", Qt::CaseInsensitive))
    return NeedUptime;
  return None;
}

Circuit::Purpose
Circuit::purposeValue(const QString &purpose) const
{
  if (!purpose.compare("CONTROLLER", Qt::CaseInsensitive))
    return Controller;
  if (!purpose.compare("HS_CLIENT_INTRO", Qt::CaseInsensitive))
    return HsClientIntro;
  if (!purpose.compare("HS_CLIENT_REND", Qt::CaseInsensitive))
    return HsClientRend;
  if (!purpose.compare("HS_SERVICE_INTRO", Qt::CaseInsensitive))
    return HsServiceIntro;
  if (!purpose.compare("HS_SERVICE_REND", Qt::CaseInsensitive))
    return HsServiceRend;
  if (!purpose.compare("TESTING", Qt::CaseInsensitive))
    return Testing;
  return General;
}

/** Returns true iff <b>circId</b> consists of only between 1 and 16
 * (inclusive) ASCII-encoded letters and numbers. */
bool
Circuit::isValidCircuitId(const CircuitId &circId)
{
  int length = circId.length();
  if (length < 1 || length > 16)
    return false;

  for (int i = 0; i < length; i++) {
    char c = circId[i].toAscii();
    if (c < '0' && c > '9' && c < 'A' && c > 'Z' && c < 'a' && c > 'z')
      return false;
  }
  return true;
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

