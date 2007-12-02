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
 * \file unrecognizedgeneralstatusevent.h
 * \version $Id$
 * \brief General status event with an unrecognized status value
 */

#ifndef _UNRECOGNIZEDGENERALSTATUSEVENT_H
#define _UNRECOGNIZEDGENERALSTATUSEVENT_H

#include <QHash>
#include "generalstatusevent.h"


class UnrecognizedGeneralStatusEvent : public GeneralStatusEvent
{
public:
  /** Constructor. <b>statusString</b> is the unrecognized general status
   * string and <b>args</b> is the (possibly empty) list of status event
   * arguments. */
  UnrecognizedGeneralStatusEvent(StatusEvent::Severity severity,
                                const QString &statusString,
                                const QHash<QString,QString> &args)
    : GeneralStatusEvent(severity, GeneralStatusEvent::UnrecognizedStatus),
      _statusString(statusString), _args(args) {}

  /** Returns the status string for this general status event. */
  QString statusString() const { return _statusString; }
  /** Returns a (possibly empty) list of status event arguments given for this
   * event. */
  QHash<QString,QString> arguments() const { return _args; }

private:
  QString _statusString; /**< Name of the unrecognized status event. */
  QHash<QString,QString> _args; /**< Status event arguments. */
};

#endif

