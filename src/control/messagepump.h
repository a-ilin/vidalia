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

#ifndef _EVENTPUMP_H
#define _EVENTPUMP_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QLinkedList>

#include "controlconnection.h"
#include "torevents.h"

class MessagePump : public QThread
{
public:
  MessagePump(ControlConnection *conn);
  ~MessagePump();
  
  /** Start the event processing thread */
  void start();
  /** Stop the event processing thread */
  void stop();

  /** Sends a control command and waits for the reply */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);


private:
  /** Pointer to a previously constructed control conn */
  ControlConnection *_conn;
  /** Set to true while the event loop is running */
  bool _run; 
  /** Lock around the waiting message queue */
  QMutex _mutex;
  /** A wait condition that awakens threads when a non-event messag arrives */
  QWaitCondition _waitCond;

  /** Implements the event processing loop that runs in a separate thread */
  void run();
 
  /** This class uses the same producer/consumer model in the Java control
   * library povided with Tor */
  class Waiter
  {
    public:
      Waiter(QMutex *mutex, QWaitCondition *waitCond);
      ControlReply getReply();
      void setReply(ControlReply reply);

    private:
      bool _isSet;
      QMutex *_mutex;
      QWaitCondition *_waitCond;
      ControlReply _reply;
  };
  /** A queue of messages waiting for a response */
  QLinkedList<Waiter*> _queue;
};

#endif

