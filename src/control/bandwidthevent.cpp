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
 * \file bandwidthevent.cpp
 * \version $Id$
 */

#include "eventtype.h"
#include "bandwidthevent.h"

/** Default constructor */
BandwidthEvent::BandwidthEvent(quint64 bytesRead, quint64 bytesWritten)
 : QEvent((QEvent::Type)CustomEventType::BandwidthEvent)
{
  _bytesRead    = bytesRead;
  _bytesWritten = bytesWritten;
}

/** Return the number of bytes read in the last second */
quint64
BandwidthEvent::bytesRead()
{
  return _bytesRead;
}

/** Returns the number of bytes written in the last second */
quint64
BandwidthEvent::bytesWritten()
{
  return _bytesWritten;
}

