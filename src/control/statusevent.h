/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
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
 * \file statusevent.h
 * \version $Id$
 * \brief Base class for all Tor status events
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

