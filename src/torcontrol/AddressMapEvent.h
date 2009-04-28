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
** \file AddressMapEvent.h
** \version $Id$
** \brief Event dispatched upon receiving a new or updated address mapping
** from Tor.
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

  /** Returns the source address for this address mapping. */
  QString from() const { return _from; }
  /** Returns the target address for this address mapping. */
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

