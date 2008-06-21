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
#include "bootstrapstatus.h"


class BootstrapStatusEvent : public ClientStatusEvent
{
public:
  /** Constructor. */
  BootstrapStatusEvent(const BootstrapStatus &status)
   : ClientStatusEvent(status.severity(), ClientStatusEvent::Bootstrap),
     _bootstrapStatus(status) {}

  /** Returns the BootstrapStatus enum value indicated by this bootstrap
   * status event. */
  BootstrapStatus status() const { return _bootstrapStatus; }

  /** Returns an integer between 0 and 100 representing an estimate of how
   * much of Tor's bootstrapping process it has completed. */
  int percentComplete() const { return status().percentComplete(); }

  /** Returns a description of Tor's current bootstrapping status. */
  QString description() const { return status().description(); }
  
  /** Returns a description of the most recent error Tor encountered while
   * attempting to bootstrap, if this event's severity is 'warn'. Otherwise,
   * this returns a default-constructed QString. */
  QString warning() const { return status().warning(); }

  /** Returns a ConnectionStatusReason enum value describing the most recent
   * error Tor encountered while attempting to bootstrap, if this event's
   * severity is 'warn'. Otherwise, this simply returns
   * tc::UnrecognizedReason. */
  tc::ConnectionStatusReason reason() const { return status().reason(); }

  /** Returns the action that the Tor software recommended be taken in
   * response to this bootstrap status event. */ 
  BootstrapStatus::Recommendation recommendedAction() const { 
    return status().recommendedAction();
  }

private:
  /** Current bootstrapping status value.
   * \sa status
   */ 
  BootstrapStatus _bootstrapStatus;
};

#endif

