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
  _status   = UnrecognizedStatus;
  _action   = UnrecognizedRecommendation;
  _percentComplete = -1;
}

/** Constructor. */
BootstrapStatus::BootstrapStatus(tc::Severity severity, Status status,
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

/** Converts a string TAG value to a BootstrapStatus enum value. */
BootstrapStatus::Status
BootstrapStatus::statusFromString(const QString &str)
{
  if (!str.compare("CONN_DIR", Qt::CaseInsensitive))
    return ConnectingToDirMirror;
  if (!str.compare("HANDSHAKE_DIR", Qt::CaseInsensitive))
    return HandshakingWithDirMirror;
  if (!str.compare("ONEHOP_CREATE", Qt::CaseInsensitive))
    return CreatingOneHopCircuit;
  if (!str.compare("REQUESTING_STATUS", Qt::CaseInsensitive))
    return RequestingNetworkStatus;
  if (!str.compare("LOADING_STATUS", Qt::CaseInsensitive))
    return LoadingNetworkStatus;
  if (!str.compare("LOADING_KEYS", Qt::CaseInsensitive))
    return LoadingAuthorityCertificates;
  if (!str.compare("REQUESTING_DESCRIPTORS", Qt::CaseInsensitive))
    return RequestingDescriptors;
  if (!str.compare("LOADING_DESCRIPTORS", Qt::CaseInsensitive))
    return LoadingDescriptors;
  if (!str.compare("CONN_OR", Qt::CaseInsensitive))
    return ConnectingToEntryGuard;
  if (!str.compare("HANDSHAKE_OR", Qt::CaseInsensitive))
    return HandshakingWithEntryGuard;
  if (!str.compare("CIRCUIT_CREATE", Qt::CaseInsensitive))
    return EstablishingCircuit;
  if (!str.compare("DONE", Qt::CaseInsensitive))
    return BootstrappingDone;
  return UnrecognizedStatus;
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
          && _status != UnrecognizedStatus
          && _percentComplete >= 0);
}

