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
** \file unrecognizedserverstatusevent.h
** \version $Id$
** \brief Server status event with an unrecognized status value
*/

#ifndef _UNRECOGNIZEDSERVERSTATUSEVENT_H
#define _UNRECOGNIZEDSERVERSTATUSEVENT_H

#include <QHash>
#include "serverstatusevent.h"


class UnrecognizedServerStatusEvent : public ServerStatusEvent
{
public:
  /** Constructor. <b>statusString</b> is the unrecognized server status value
   * string we and <b>args</b> is the (possibly empty) list of status event
   * arguments. */
  UnrecognizedServerStatusEvent(tc::Severity severity,
                                const QString &statusString,
                                const QHash<QString,QString> &args)
    : ServerStatusEvent(severity, ServerStatusEvent::UnrecognizedStatus),
      _statusString(statusString), _args(args) {}

  /** Returns the status string for this server status event. */
  QString statusString() const { return _statusString; }
  /** Returns a (possibly empty) list of status event arguments given for this
   * event. */
  QHash<QString,QString> arguments() const { return _args; }

private:
  QString _statusString; /**< Name of the unrecognized status event. */
  QHash<QString,QString> _args; /**< Status event arguments. */
};

#endif

