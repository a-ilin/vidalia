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
** \file StatusEvent.h
** \version $Id$
** \brief Base class for all Tor status events
*/

#ifndef _STATUSEVENT_H
#define _STATUSEVENT_H

#include "eventtype.h"
#include "tcglobal.h"

#include <QEvent>
#include <QString>


class StatusEvent : public QEvent
{
public:
  /** Constructor */
  StatusEvent(QEvent::Type type, tc::Severity severity)
    : QEvent(type), _severity(severity) {}

  /** Returns the severity of this status event. */
  tc::Severity severity() const { return _severity; }
  /** Returns a StatusEvent::Severity enum value for the severity represented
   * by <b>str</b>. */ 
  static tc::Severity severityFromString(const QString &str);

private:
  tc::Severity _severity; /**< Severity of this status event. */
};

#endif

