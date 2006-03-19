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
 * \file logevent.h
 * \version $Id$
 */

#ifndef _LOGEVENT_H
#define _LOGEVENT_H

#include <QString>
#include <QEvent>
#include <config/messagetypes.h>


class LogEvent : public QEvent
{
public:
  /** Log message severity levels */
  enum Severity {
    Unknown = 0,
    TorDebug      = TOR_DEBUG,  /**< Debug level log message. */
    TorInfo       = TOR_INFO,   /**< Info level log message. */
    TorNotice     = TOR_NOTICE, /**< Notice level log message. */
    TorWarn       = TOR_WARN,   /**< Warn level log message. */
    TorError      = TOR_ERROR,  /**< Error level log message. */
  };
  
  /** Default constructor */
  LogEvent(Severity severity, QString message);
  
  /** Converts the string description of a severity to its enum value */
  static Severity toSeverity(QString strSeverity);

  /** Converts the Severity enum value to a string description */
  static QString severityToString(Severity severity);

  /** Returns the severity of this log event */
  Severity severity();
  
  /** Returns the message for this log event */
  QString message();
  
private:
  Severity _severity;
  QString  _message;
};

#endif
