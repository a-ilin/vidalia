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
** \file DangerousVersionEvent.h
** \version $Id$
** \brief Event sent when Tor realizes its version is not recommended
*/

#ifndef _DANGEROUSPORTEVENT_H
#define _DANGEROUSPORTEVENT_H

#include "ClientStatusEvent.h"
#include "tcglobal.h"

#include <QString>


class DangerousPortEvent : public ClientStatusEvent
{

public:
  /** Possible actions that Tor takes in response to a client connection
   * to a plaintext port.
   */
  enum Result {
    UnrecognizedResult,
    Reject,
    Warn,
  };
  
  /** Constructs a new DangerousPortEvent object, using <b>severity</b>
   * as the event severity level. <b>port</b> and <b>result</b> describe
   * the port on which a potential plaintext connection was attempted
   * and the action Tor took in response, respectively.
   */
  DangerousPortEvent(tc::Severity severity, quint16 port, Result result);

  /** Returns the DangerousPortEvent::Result enum value specified in this
   * event's constructor.
   */
  Result result() const { return _result; }

  /** Returns the port specified in this event's constructor.
   */
  quint16 port() const { return _port; }

  /** Returns an DangerousPortEvent::Result enum value corresponding to the
   * event result described in <b>str</b>, or UnrecognizedResult if <b>str</b>
   * does not contain a valid Result type.
   */
  static Result resultFromString(const QString &str);

private:
  quint16 _port;
  Result _result;
};

#endif

