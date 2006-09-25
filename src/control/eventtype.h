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
 * \file eventtype.h
 * \version $Id$
 * \brief Custom events dispatched upon receiving notification from Tor
 */

#ifndef _EVENTTYPE_H
#define _EVENTTYPE_H

#include <QEvent>

namespace CustomEventType {
  const int BandwidthEvent = QEvent::User;
  const int CircuitEvent   = QEvent::User+1;
  const int StreamEvent    = QEvent::User+2;
  const int LogEvent       = QEvent::User+3;
  const int OrConnEvent    = QEvent::User+4;
  const int NewDescriptorEvent = QEvent::User+5;
}

#endif

