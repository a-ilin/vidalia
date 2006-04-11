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
 * \file torsignal.cpp
 * \version $Id$
 */

#include "torsignal.h"

/** Returns a string representation of a signal that Tor understands. */
QString
TorSignal::toString(Signal signal)
{
  QString sigtype;
  switch (signal) {
    case Reload:    sigtype = "RELOAD"; break;
    case Shutdown:  sigtype = "SHUTDOWN"; break;
    case Dump:      sigtype = "DUMP"; break;
    case Debug:     sigtype = "DEBUG"; break;
    case Halt:      sigtype = "HALT"; break;
    case NewNym:    sigtype = "NEWNYM"; break;
    default:  return "UNKNOWN";
  }
  return sigtype;
}

