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
** \file dangerousportevent.h
** \version $Id$
** \brief Event sent when the client makes a connection to a port listed in
** their WarnPlaintextPorts or RejectPlaintextPorts settings.
*/

#include "dangerousportevent.h"


DangerousPortEvent::DangerousPortEvent(tc::Severity severity, quint16 port,
                                       Result result)
  : ClientStatusEvent(severity, ClientStatusEvent::DangerousPort),
    _port(port),
    _result(result)
{
}

DangerousPortEvent::Result
DangerousPortEvent::resultFromString(const QString &str)
{
  if (!str.compare("WARN", Qt::CaseInsensitive))
    return DangerousPortEvent::Warn;
  if (!str.compare("REJECT", Qt::CaseInsensitive))
    return DangerousPortEvent::Reject;
  return DangerousPortEvent::UnrecognizedResult;
}

