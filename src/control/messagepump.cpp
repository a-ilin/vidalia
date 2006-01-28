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
  /* Let the loop in run() know that we want to stop */
  _run = false;
  
  /* Fill all waiters with an empty reply */
  foreach (Waiter *w, _queue) {
    w->setReply(ControlReply());
  }

  /* If the thread is running, then wait for it to terminate */
  if (isRunning()) {
    _mutex.lock();
    _waitCond.wait(&_mutex);
    _mutex.unlock();
  }
}

/** Sends a message and waits for the response from Tor */
bool
MessagePump::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  _mutex.lock();
  if (_conn->sendCommand(cmd, errmsg)) {
    if (_run) {
      /* Add a new waiter and add it to the queue */
      Waiter *w = new Waiter(&_mutex, &_waitCond);
      _queue.enqueue(w);
      _mutex.unlock();
  
      /* Wait for the response message */
      reply = w->getReply();
      delete w;
      return true; 
    } else {
      /* The thread isn't running, so just read the reply normally */
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
  forever {
    /* If we're supposed to stop or the connection broke, then bail */
    if (!_run || !_conn->isConnected()) {
      break;
    }
    
    /* Check if data is waiting to be read */
    if (_conn->canReadLine()) {
      ControlReply reply;
      /* There is data on the socket, so read the reply message */
      if (_conn->readReply(reply)) {
        if (reply.getStatus() == "650") {
          /* A status of "650" means "asynchronous event notification, so
           * let's go ahead and dispatch the event. */
          _events->handleEvent(reply);
        } else {
          /* Remove the first waiter from the queue of messages awaiting a
           * response. */ 
          _mutex.lock();
          Waiter *w = _queue.dequeue();
          _mutex.unlock();
 
          /* Give it the message we just received */
          w->setReply(reply);
        }
      }
    } else {
      /* No data is on the socket, so wait a little bit and then try again */
      _conn->waitForReadyRead(250);
    }
  }
  /* Let stop() know that we're exiting now */
  _waitCond.wakeAll();
  _run = false;
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

