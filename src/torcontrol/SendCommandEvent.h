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
** \file SendCommandEvent.h
** \brief An event posted to a socket living in another thread, indicating
** that it should send the given control command.
*/

#ifndef _SENDCOMMANDEVENT_H
#define _SENDCOMMANDEVENT_H

#include "ControlCommand.h"

#include <QEvent>
#include <QMutex>
#include <QWaitCondition>


class SendCommandEvent : public QEvent {
public:
  /** Object used to wait for the result of a send operation. */
  class SendWaiter {
  public:
    /** Status of the send SendWaiter. */
    enum SenderStatus { Waiting, Failed, Success } _status;
    /** Default constructor. */
    SendWaiter() { _status = Waiting; }
    /** Sets the result of the send operation. */
    void setResult(bool success, const QString &errmsg = QString());
    /** Waits for and returns the result of the send operation. */
    bool getResult(QString *errmsg = 0);
    /** Returns the SendWaiter's current SenderStatus value. */
    SenderStatus status();
  private:
    QMutex _mutex; /**< Mutex around the wait condition. */
    QWaitCondition _waitCond; /**< Waits for the send to complete. */
    QString _errmsg; /**< Error message if the send fails. */
  };

  /** Constructor. */
  SendCommandEvent(const ControlCommand &cmd, SendWaiter *w = 0);
  /** Returns the control command to send to Tor. */
  ControlCommand command() { return _cmd; }
  /** Returns a SendWaiter (if any) for the result of this send. */
  SendWaiter* waiter() { return _waiter; }

private:
  ControlCommand _cmd;  /**< Command to send to Tor. */
  SendWaiter* _waiter; /**< SendWaiter for the result of this event. */
};

#endif
