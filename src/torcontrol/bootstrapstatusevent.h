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
** \file bootstrapstatusevent.h
** \version $Id$
** \brief Event sent by Tor when its bootstrapping status changes
*/

#ifndef _BOOTSTRAPSTATUSEVENT_H
#define _BOOTSTRAPSTATUSEVENT_H

#include <QEvent>
#include <QString>
#include "clientstatusevent.h"
#include "tcglobal.h"


class BootstrapStatusEvent : public ClientStatusEvent
{
public:
  /** Currently enumerated bootstrapping states defined by Tor's control
   * protocol (Tor >= 0.2.1.0-alpha-dev. */ 
  enum BootstrapStatus {
    UnrecognizedStatus,
    ConnectingToDirMirror,
    HandshakingWithDirMirror,
    CreatingOneHopCircuit,
    RequestingNetworkStatus,
    LoadingNetworkStatus,
    LoadingAuthorityCertificates,
    RequestingDescriptors,
    LoadingDescriptors,
    ConnectingToEntryGuard,
    HandshakingWithEntryGuard,
    EstablishingCircuit,
    BootstrappingDone
  };
  /** Actions the Tor software might recommend controllers take in response to
   * a bootstrap status problem event. */
  enum Recommendation {
    UnrecognizedRecommendation,
    RecommendIgnore,
    RecommendWarn
  };

  /** Constructor. */
  BootstrapStatusEvent(StatusEvent::Severity severity,
                       BootstrapStatus status, int percentComplete,
                       const QString &description,
                       const QString &warning = QString(),
                       tc::ConnectionStatusReason reason = tc::UnrecognizedReason,
                       Recommendation action = UnrecognizedRecommendation);

  /** Returns the BootstrapStatus enum value indicated by this bootstrap
   * status event. */
  BootstrapStatus status() const { return _status; }

  /** Returns an integer between 0 and 100 representing an estimate of how
   * much of Tor's bootstrapping process it has completed. */
  int percentComplete() const { return _percentComplete; }

  /** Returns a description of Tor's current bootstrapping status. */
  QString description() const { return _description; }
  
  /** Returns a description of the most recent error Tor encountered while
   * attempting to bootstrap, if this event's severity is 'warn'. Otherwise,
   * this returns a default-constructed QString. */
  QString warning() const { return _warning; }

  /** Returns a ConnectionStatusReason enum value describing the most recent
   * error Tor encountered while attempting to bootstrap, if this event's
   * severity is 'warn'. Otherwise, this simply returns
   * tc::UnrecognizedReason. */
  tc::ConnectionStatusReason reason() const { return _reason; }

  /** Returns the action that the Tor software recommended be taken in
   * response to this bootstrap status event. */ 
  Recommendation recommendedAction() const { return _action; }

  /** Converts a string TAG value to a BootstrapStatus enum value. */
  static BootstrapStatus statusFromString(const QString &tag);
  /** Converts a string RECOMMENDATION value to a RecommendAction enum
   * value. */
  static Recommendation actionFromString(const QString &str);

private:
  /** Current bootstrapping status value.
   * \sa status
   */ 
  BootstrapStatus _status;
  
  /** Approximate percentage of Tor's bootstrapping process that is complete.
   * \sa percentComplete
   */
  int _percentComplete;
  
  /** Description of Tor's current bootstrapping status.
   * \sa description
   */
  QString _description;
  
  /** Description of the most recent error Tor encountered while attempting to
   * bootstrap. 
   * \sa warning
   */
  QString _warning;

  /** ConnectionStatusReason enum value describing the most recent error Tor
   * encountered while attempting to bootstrap.
   * \sa reason
   */
  tc::ConnectionStatusReason _reason;

  /** Recommendation enum value describing Tor's suggested response to this
   * bootstrap status event.
   * \sa recommendedAction
   */
  Recommendation _action;
};

#endif

