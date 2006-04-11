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
 * \file torsignal.h
 * \version $Id$
 */

#ifndef _TORSIGNAL_H
#define _TORSGINAL_H

#include <QString>


class TorSignal
{
public:
  /** Signals that can be sent by the controller */
  enum Signal {
    Reload,   /**< SIGHUP: Reloads config items and refetch directory */
    Shutdown, /**< SIGINT: Controlled shutdown */
    Dump,     /**< SIGUSR1: Log information about current circuits */
    Debug,    /**< SIGUSR2: Switch all open logs to loglevel debug */
    Halt,     /**< SIGTERM: Immediate shutdown */
    NewNym    /**< NEWNYM: Switch to all new clean circuits */
  };
  
  /** Returns a string representation of a signal that Tor understands. */
  static QString toString(Signal signal);
};

#endif

