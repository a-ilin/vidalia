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
** \file eventtype.h
** \version $Id$
** \brief Custom events dispatched upon receiving notification from Tor
*/

#ifndef _EVENTTYPE_H
#define _EVENTTYPE_H

#include <QEvent>


/** Custom event type identifiers. */
namespace CustomEventType {
  /** Bandwidth update event. */
  const int BandwidthEvent = QEvent::User;
  /** Circuit status event. */
  const int CircuitEvent   = QEvent::User+1;
  /** Stream status event. */
  const int StreamEvent    = QEvent::User+2;
  /** Log message event. */
  const int LogEvent       = QEvent::User+3;
  /** OR connection status event. */
  const int OrConnEvent    = QEvent::User+4;
  /** New descriptor event. */
  const int NewDescriptorEvent = QEvent::User+5;
  /** Control command send event. */
  const int SendCommandEvent   = QEvent::User+6;
  /** New address mapping event. */
  const int AddressMapEvent = QEvent::User+7;
  /** Tor client status events. */
  const int ClientStatusEvent = QEvent::User+8;
  /** Tor server status events. */
  const int ServerStatusEvent = QEvent::User+9;
  /** General Tor status events. */
  const int GeneralStatusEvent = QEvent::User+10;
}

#endif

