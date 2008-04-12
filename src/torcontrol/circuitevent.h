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
** \file circuitevent.h
** \version $Id$
** \brief Event dispatched containing updated circuit status information
*/

#ifndef _CIRCUITEVENT_H
#define _CIRCUITEVENT_H

#include <QEvent>

#include "eventtype.h"
#include "circuit.h"


class CircuitEvent : public QEvent
{
public:
  /** Constructor */
  CircuitEvent(Circuit circuit)
  : QEvent((QEvent::Type)CustomEventType::CircuitEvent)
  { _circuit = circuit; }

  /** Returns the Circuit object for this event. */
  Circuit circuit() const { return _circuit; }
  /** Returns the ID for this circuit event. */
  quint64 id() const { return _circuit.id(); }
  /** Returns the status of this circuit event. */
  Circuit::Status status() const { return _circuit.status(); }
  /** Returns the names of the routers in the path for this circuit event. */
  QStringList routerNames() const { return _circuit.routerNames(); }
  /** Returns the IDs of the routers in the path for this circuit event. */
  QStringList routerIDs() const { return _circuit.routerIDs(); }
  
private:
  /** Circuit object for this event. */
  Circuit _circuit;
};

#endif

