/****************************************************************
*  $Id$
* 
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

#ifndef _LOGEVENT_H
#define _LOGEVENT_H

#include <QString>
#include <QEvent>
#include "../config/messagetypes.h"

class LogEvent : public QEvent
{
public:
  /** Log message severity levels */
  enum Severity {
    Unknown = 0,
    TorDebug      = TOR_DEBUG,
    TorInfo       = TOR_INFO,
    TorNotice     = TOR_NOTICE,
    TorWarn       = TOR_WARN,
    TorError      = TOR_ERROR,
    VidaliaDebug  = VIDALIA_DEBUG,
    VidaliaInfo   = VIDALIA_INFO,
    VidaliaNotice = VIDALIA_NOTICE,
    VidaliaWarn   = VIDALIA_WARN,
    VidaliaError  = VIDALIA_ERROR
  };
  
  /** Default constructor */
  LogEvent(Severity severity, QString message);
  /** Default destructor */
  ~LogEvent();
  
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
