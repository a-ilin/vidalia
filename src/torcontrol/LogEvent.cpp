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
** \file LogEvent.cpp
** \brief Event dispatched containing a log message from Tor
*/

#include "LogEvent.h"
#include "eventtype.h"


/** Default constructor */
LogEvent::LogEvent(Severity severity, QString message)
: QEvent((QEvent::Type)CustomEventType::LogEvent)
{
  _severity = severity;
  _message  = message.trimmed();
}

/** Converts a string description of a severity to its enum value */
LogEvent::Severity
LogEvent::toSeverity(QString strSeverity)
{
  Severity s;
  strSeverity = strSeverity.toUpper();
  if (strSeverity == "DEBUG") {
    s = Debug;
  } else if (strSeverity == "INFO") {
    s = Info;
  } else if (strSeverity == "NOTICE") {
    s = Notice;
  } else if (strSeverity == "WARN") {
    s = Warn;
  } else if (strSeverity == "ERR" || strSeverity == "ERROR") {
    s = Error;
  } else {
    s = Unknown;
  }
  return s;
}

/** Converts a Severity enum value to a string description */
QString
LogEvent::severityToString(Severity s)
{
  QString str;
  switch (s) {
    case Debug:  str = tr("Debug"); break;
    case Info:   str = tr("Info"); break;
    case Notice: str = tr("Notice"); break;
    case Warn:   str = tr("Warning"); break;
    case Error:  str = tr("Error"); break;
    default: str = tr("Unknown"); break;
  }
  return str;
}

/** Returns the severity of this log event */
LogEvent::Severity
LogEvent::severity() const
{
  return _severity;
}

/** Returns the message for this log event */
QString
LogEvent::message() const
{
  return _message;
}

