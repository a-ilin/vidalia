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
** \file TorSignal.cpp
** \brief Sends a signal to Tor's control interface
*/

#include "TorSignal.h"


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

