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
** \file RouterStatus.cpp
** \brief Parses a blob of router status text from Tor
*/

#include "RouterStatus.h"

#include "stringutil.h"

/** Defines the time format used when parsing the published date and time from
 * a router's status. */
#define TIME_FORMAT  "yyyy-MM-dd HH:mm:ss"


/** Constructor. Parses <b>status</b> for router status information. The given
 * string should match the router status entry format from Tor's dir-spec.txt.
 * The currently recognized lines are:
 *
 *  "r" SP nickname SP identity SP digest SP publication SP IP SP ORPort
 *      SP DirPort NL
 *  "s" SP Flags NL
 *
 * Unrecognized lines are currently ignored.
 *
 * */
RouterStatus::RouterStatus(const QStringList &status)
{
  bool ok;

  _valid = false;
  _flags = 0;

  _bandwidth = 0;

  foreach (QString line, status) {
    if (line.startsWith("r ")) {
      QStringList parts = line.split(" ", QString::SkipEmptyParts);
      if (parts.size() < 9)
        return;

      /* Nickname */
      _name = parts.at(1);
      /* Identity key digest */
      _id = base16_encode(QByteArray::fromBase64(parts.at(2).toAscii()));
      if (_id.isEmpty())
        return;
      /* Most recent descriptor digest */
      _digest = base16_encode(QByteArray::fromBase64(parts.at(3).toAscii()));
      if (_digest.isEmpty())
        return;
      /* Most recent publication date */
      _published = QDateTime::fromString(parts.at(4) + " " + parts.at(5),
                                         TIME_FORMAT);
      if (!_published.isValid())
        return;
      /* IP address */
      _ipAddress = QHostAddress(parts.at(6));
      if (_ipAddress.isNull())
        return;
      /* ORPort */
      _orPort = parts.at(7).toUInt(&ok);
      if (!ok)
        return;
      /* DirPort */
      _dirPort = parts.at(8).toUInt(&ok);
      if (!ok)
        return;

      _valid = true;
    } else if (line.startsWith("s ")) {
      /* Status flags */
      QStringList flags = line.split(" ", QString::SkipEmptyParts);
      flags.removeFirst(); /* Remove the "s" */

      foreach (QString flag, flags) {
        _flags |= flagValue(flag);
      }
    } else if (line.startsWith("w ")) {
      /* Status flags */
      QStringList parts = line.split(" ", QString::SkipEmptyParts);
      parts.removeFirst(); /* Remove the "w" */

      if(parts.size() > 0) {
        QStringList bw = parts.at(0).split("=", QString::SkipEmptyParts);
        if(bw.size() == 2) {
          _bandwidth = (quint64)bw.at(1).toULongLong();
        }
      }
    }
  }
}

/** Returns a Flags enum value for the given router status <b>flag</b>. If
 * <b>flag</b> is not recognized, then <i>Unknown</i> is returned. */
RouterStatus::Flag
RouterStatus::flagValue(const QString &flag)
{
  if (!flag.compare("Authority", Qt::CaseInsensitive))
    return Authority;
  if (!flag.compare("BadExit", Qt::CaseInsensitive))
    return BadExit;
  if (!flag.compare("BadDirectory", Qt::CaseInsensitive))
    return BadDirectory;
  if (!flag.compare("Exit", Qt::CaseInsensitive))
    return Exit;
  if (!flag.compare("Fast", Qt::CaseInsensitive))
    return Fast;
  if (!flag.compare("Guard", Qt::CaseInsensitive))
    return Guard;
  if (!flag.compare("HSDir", Qt::CaseInsensitive))
    return HSDir;
  if (!flag.compare("Named", Qt::CaseInsensitive))
    return Named;
  if (!flag.compare("Running", Qt::CaseInsensitive))
    return Running;
  if (!flag.compare("Stable", Qt::CaseInsensitive))
    return Stable;
  if (!flag.compare("Valid", Qt::CaseInsensitive))
    return Valid;
  if (!flag.compare("V2Dir", Qt::CaseInsensitive))
    return V2Dir;
  if (!flag.compare("V3Dir", Qt::CaseInsensitive))
    return V3Dir;
  return Unknown; /* Unknown status flag */
}

