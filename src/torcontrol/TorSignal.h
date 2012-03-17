/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file TorSignal.h
** \brief Sends a signal to Tor's control interface
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

