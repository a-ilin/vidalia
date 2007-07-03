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
 * \file addressmapevent.h
 * \version $Id: bandwidthevent.h 1563 2006-12-26 06:06:04Z edmanm $
 * \brief Event dispatched upon receiving a new or updated address mapping
 * from Tor.
 */

#ifndef _ADDRESSMAPEVENT_H
#define _ADDRESSMAPEVENT_H

#include <QEvent>
#include <QString>
#include <QDateTime>


class AddressMapEvent : public QEvent
{
public:
  /** Constructor */
  AddressMapEvent(QString from, QString to, QDateTime expires)
  : QEvent((QEvent::Type)CustomEventType::AddressMapEvent)
  { _from = from; _to = to; _expires = expires; }

  /** Returns the number of bytes read in the last second */
  QString from() const { return _from; }
  /** Returns the number of bytes written in the last second */
  QString to() const { return _to; }
  /** Returns the date and time at which this mapping should expire, in local
   * time. */
  QDateTime expires() const { return _expires; }
  /** Returns true if this address mapping is expired. */
  bool isExpired() const { return (_expires > QDateTime::currentDateTime()); }

private:
  QString _from; /**< The source address of this mapping. */
  QString _to;   /**< The target address of this mapping. */
  QDateTime _expires; /**< The time at which this mapping expires (if ever). */
};

#endif

