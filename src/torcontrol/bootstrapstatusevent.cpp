/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/* 
** \file bootstrapstatusevent.cpp
** \version $Id$
** \brief Event sent by Tor when its bootstrapping status changes
*/

#include "bootstrapstatusevent.h"


/** Constructor. */
BootstrapStatusEvent::BootstrapStatusEvent(StatusEvent::Severity severity,
                                           BootstrapStatus status,
                                           int percentComplete,
                                           const QString &description,
                                           const QString &warning,
                                           tc::ConnectionStatusReason reason)
  : ClientStatusEvent(severity, ClientStatusEvent::BootstrapStatus)
{
  _status = status;
  _percentComplete = qBound(0, percentComplete, 100);
  _description = description;
  _warning = warning;
  _reason = reason;
}

/** Converts a string TAG value to a BootstrapStatus enum value. */
BootstrapStatusEvent::BootstrapStatus
BootstrapStatusEvent::statusFromString(const QString &str)
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

