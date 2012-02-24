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
** \file SendEommandEvent.cpp
** \brief An event posted to a socket living in another thread, indicating
** that it should send the given control command.
*/

#include "SendCommandEvent.h"

#include <QCoreApplication>

SendCommandEvent::SendCommandEvent(const ControlCommand &cmd, SendWaiter *w)
  : QEvent(QEvent::User)
{
  _cmd    = cmd;
  _waiter = w;
}

/** Sets the result of the send operation. */
void
SendCommandEvent::SendWaiter::setResult(bool success, const QString &errmsg)
{
  _status = (success ? Success : Failed);
  _errmsg = errmsg;
}

/** Waits for and gets the result of the send operation. */
bool 
SendCommandEvent::SendWaiter::getResult(QString *errmsg)
{
  while(_status == Waiting) {
    QCoreApplication::processEvents();
  }
  if (errmsg) {
    *errmsg = _errmsg;
  }
  return (_status == Success);
}

/** Returns the SendWaiter's current SenderStatus value. */
SendCommandEvent::SendWaiter::SenderStatus
SendCommandEvent::SendWaiter::status()
{
  return _status;
}
