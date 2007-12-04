/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
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
 * \file dangerousversionevent.h
 * \version $Id$
 * \brief Event sent when Tor realizes its version is not recommended
 */

#ifndef _DANGEROUSVERSIONEVENT_H
#define _DANGEROUSVERSIONEVENT_H

#include <QStringList>
#include "generalstatusevent.h"


class DangerousVersionEvent : public GeneralStatusEvent
{
public:
  /** Possible reasons Tor thinks its version is dangerous. */
  enum Reason {
    UnrecognizedReason, 
    NewVersion, /**< The current version is newer than any recommended
                     version. */
    OldVersion, /**< The current version is older than any recommended 
                     version. */
    UnrecommendedVersion /**< The current version of Tor should not be used. */
  };

  /** Constructor. */
  DangerousVersionEvent(StatusEvent::Severity severity, Reason reason,
                        const QString &currentVersion,
                        const QStringList &recommendedVersions)
    : GeneralStatusEvent(severity, GeneralStatusEvent::DangerousTorVersion),
        _reason(reason), 
        _current(currentVersion), 
        _recommended(recommendedVersions) {}

  /** Returns the Reason enum value indicating the reason Tor thinks its
   * version is dangerous. */
  Reason reason() const { return _reason; }
  /** Returns the user's current Tor version. */
  QString currentVersion() const { return _current; }
  /** Returns a list of recommended Tor versions. */
  QStringList recommendedVersions() const { return _recommended; }

  /** Returns a DangerousVersionEvent::Reason enum value for <b>str</b>,
   * representing the reason why Tor thinks its version is dangerous. */
  static Reason reasonFromString(const QString &str);

private:
  Reason _reason; /**< Reason this Tor version is dangerous. */
  QString _current; /**< The user's current Tor version. */
  QStringList _recommended; /**< A list of recommended Tor versions. */
};

#endif

