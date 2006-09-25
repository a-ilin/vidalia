/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
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
 * \file circuitevent.h
 * \version $Id$
 * \brief Event dispatched containing updated circuit status information
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
  Circuit circuit() { return _circuit; }
  /** Returns the ID for this circuit event. */
  quint64 id() { return _circuit.id(); }
  /** Returns the status of this circuit event. */
  Circuit::Status status() { return _circuit.status(); }
  /** Returns the path chosen for this circuit event.  */
  QString path() { return _circuit.path(); }
  
private:
  /** Circuit object for this event. */
  Circuit _circuit;
};

#endif

