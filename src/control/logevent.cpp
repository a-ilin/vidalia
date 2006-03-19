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
 * \file logevent.cpp
 * \version $Id$
 */

#include "eventtype.h"
#include "logevent.h"

/** Default constructor */
LogEvent::LogEvent(Severity severity, QString message)
: QEvent((QEvent::Type)CustomEventType::LogEvent)
{
  _severity = severity;
  _message  = message;
}

/** Converts a string description of a severity to its enum value */
LogEvent::Severity
LogEvent::toSeverity(QString strSeverity)
{
  Severity s;
  strSeverity = strSeverity.toUpper();
  if (strSeverity == "DEBUG") {
    s = TorDebug;
  } else if (strSeverity == "INFO") {
    s = TorInfo;
  } else if (strSeverity == "NOTICE") {
    s = TorNotice;
  } else if (strSeverity == "WARN") {
    s = TorWarn;
  } else if (strSeverity == "ERR" || strSeverity == "ERROR") {
    s = TorError;
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
    case TorDebug:  str = MSG_TOR_DEBUG; break;
    case TorInfo:   str = MSG_TOR_INFO; break;
    case TorNotice: str = MSG_TOR_NOTICE; break;
    case TorWarn:   str = MSG_TOR_WARN; break;
    case TorError:  str = MSG_TOR_ERROR; break;
    default: str = "Unknown Error"; break;
  }
  return str;
}

/** Returns the severity of this log event */
LogEvent::Severity
LogEvent::severity()
{
  return _severity;
}

/** Returns the message for this log event */
QString
LogEvent::message()
{
  return _message;
}
