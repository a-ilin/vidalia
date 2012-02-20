/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/* 
** \file BootstrapStatus.cpp
** \brief  Describes the Tor software's current bootstrapping status
*/

#include "BootstrapStatus.h"


BootstrapStatus::BootstrapStatus()
{
  _severity = tc::UnrecognizedSeverity;
  _reason   = tc::UnrecognizedReason;
  _status   = QString();
  _action   = UnrecognizedRecommendation;
  _percentComplete = -1;
}

/** Constructor. */
BootstrapStatus::BootstrapStatus(tc::Severity severity, 
                                 const QString &status, 
                                 int percentComplete,
                                 const QString &description,
                                 const QString &warning,
                                 tc::ConnectionStatusReason reason,
                                 Recommendation action)
{
  _severity = severity;
  _status = status;
  _percentComplete = qBound(0, percentComplete, 100);
  _description = description;
  _warning = warning;
  _reason = reason;
  _action = action;
}

/** Returns the action that the Tor software recommended be taken in response
 * to this bootstrap status. */
BootstrapStatus::Recommendation
BootstrapStatus::actionFromString(const QString &str)
{
  if (!str.compare("WARN", Qt::CaseInsensitive))
    return RecommendWarn;
  if (!str.compare("IGNORE", Qt::CaseInsensitive))
    return RecommendIgnore;
  return UnrecognizedRecommendation;
}

/** Returns true if this object represents a valid bootstrap status phase. */
bool
BootstrapStatus::isValid() const
{
  return (_severity != tc::UnrecognizedSeverity
            && _percentComplete >= 0);
}

