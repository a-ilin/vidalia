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
** \file dangerousversionevent.h
** \version $Id$
** \brief Event sent when Tor realizes its version is not recommended
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
    ObsoleteVersion, /**< The current version is older than any recommended 
                          version. */
    UnrecommendedVersion /**< The current version of Tor should not be used. */
  };

  /** Constructor. */
  DangerousVersionEvent(tc::Severity severity, Reason reason,
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

