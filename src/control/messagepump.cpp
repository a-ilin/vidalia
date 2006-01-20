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

#include "messagepump.h"

/** Constructor. This message pump will process messages over the given
 * ControlConnection object. */
MessagePump::MessagePump(ControlConnection *conn, TorEvents *events)
{
  _conn   = conn;
  _events = events;
}

/** Default destructor */
MessagePump::~MessagePump()
{
  if (_run) {
    stop();
  }
}

/** Starts the event processing thread */
void
MessagePump::start()
{
  _run = true;
  QThread::start();
}

/** Stops the event processing thread */
void
MessagePump::stop()
{
  _run = false;
  /* Since the message pump thread will be blocked in readReply() most likely,
   * we either need to find a way to make QAbstractSocket break out of a 
   * blocking read, or we have to kill the thread harshly. For now, we just
   * do the latter, but it would be nice to know how to do the former. */
  QThread::terminate();
}

/** Sends a message and waits for the response from Tor */
bool
MessagePump::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  bool rc;
  _mutex.lock();
  if ((rc = _conn->sendCommand(cmd, errmsg)) == true) {
    if (_run) {
      /* Add a new waiter and add it to the queue */
      Waiter *w = new Waiter(&_mutex, &_waitCond);
      _queue.push_back(w);
      _mutex.unlock();
  
      /* Wait for the response message */
      reply = w->getReply();
      delete w;
      return true; 
    } else {
      _mutex.unlock();
      return _conn->readReply(reply, errmsg);
    }
  }
  _mutex.unlock();
  return false;
}

/** Implements the Tor event processing loop */
void
MessagePump::run()
{
  while (_run) {
    ControlReply reply;
    if (_conn->readReply(reply)) {
      /* A status of "650" means "asynchronous event notification */
      if (reply.getStatus() == "650") {
        /* Dispatch the event */
        _events->handleEvent(reply);
      } else {
        /* Remove the first waiter from the queue of messages awaiting a
         * response. */ 
        _mutex.lock();
        Waiter *w = _queue.takeFirst();
        _mutex.unlock();
 
        /* Give it the message we just received */
        w->setReply(reply);
      }
    }
  }
}


/********************************************************************
 * MessagePump::Waiter
 */
/** Default constructor of the Waiter object */
MessagePump::Waiter::Waiter(QMutex *mutex, QWaitCondition *waitCond)
{
  _mutex = mutex;
  _waitCond = waitCond;
  _isSet = false;
}

/** Wait for the reply for this message to arrive and then return */
ControlReply
MessagePump::Waiter::getReply()
{
  while (!_isSet) {
    _mutex->lock();
    _waitCond->wait(_mutex);
    _mutex->unlock();
  }
  return _reply;
}

/** The reply for this message has arrived. Set the proper member variable and
 * then signal to wake up the waiting thread. */
void
MessagePump::Waiter::setReply(ControlReply reply)
{
  _reply = reply;
  _isSet = true;
  _waitCond->wakeAll();
}

