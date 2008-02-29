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
** \file circuitestablishedevent.h
** \version $Id$
** \brief Event sent when Tor has first successfully established a circuit
*/

#ifndef _CIRCUITESTABLISHEDEVENT_H
#define _CIRCUITESTABLISHEDEVENT_H

#include "clientstatusevent.h"


class CircuitEstablishedEvent : public ClientStatusEvent
{
public:
  /** Constructor. */
  CircuitEstablishedEvent(StatusEvent::Severity severity)
    : ClientStatusEvent(severity, ClientStatusEvent::CircuitEstablished) {}
};

#endif

