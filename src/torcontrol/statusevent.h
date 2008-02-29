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
** \file statusevent.h
** \version $Id$
** \brief Base class for all Tor status events
*/

#ifndef _STATUSEVENT_H
#define _STATUSEVENT_H

#include <QEvent>
#include <QString>
#include "eventtype.h"


class StatusEvent : public QEvent
{
public:
  /** Satus event severity values. */
  enum Severity {
    SeverityUnknown,  /**< An unrecognized status event severity. */
    SeverityNotice,   /**< A not-so-bad status event. */
    SeverityWarn,     /**< An important, but non-fatal status event. */
    SeverityError     /**< A critical status event. */
  };
  
  /** Constructor */
  StatusEvent(QEvent::Type type, Severity severity)
    : QEvent(type), _severity(severity) {}

  /** Returns the severity of this status event. */
  Severity severity() const { return _severity; }
  /** Returns a StatusEvent::Severity enum value for the severity represented
   * by <b>str</b>. */ 
  static Severity severityFromString(const QString &str);

private:
  Severity _severity; /**< Severity of this status event. */
};

#endif

