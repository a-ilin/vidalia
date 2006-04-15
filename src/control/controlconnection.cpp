/****************************************************************
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

/** 
 * \file controlconnection.cpp
 * \version $Id$
 */

#include <QCoreApplication>
#include <util/string.h>

#include "controlconnection.h"


/** Maximum time to wait for new data to arrive on the socket. */
#define WAIT_TIMEOUT          10
/** Maximum number of times we'll try to connect to Tor before giving up.*/
#define MAX_CONNECT_ATTEMPTS  6
/** Time to wait between control connection attempts (in microseconds). */
#define CONNECT_RETRY_DELAY   500*1000


/** Constructor.
 * \param events a TorEvents object used to dispatch asynchronous events
 * received from Tor.
 */
ControlConnection::ControlConnection(TorEvents *events)
{
  _events = events;
}

/** Connects to Tor's control interface and starts a thread used to process
 * asynchronous event messages received from Tor. All socket communication is
 * done in the new thread, processing incoming and outgoing messages through the
 * send and receive queues.
 * \param addr IP address of Tor's control interface.
 * \param port port of Tor's control interface.
 * \param errmsg Stores an error message if the connection fails.
 * \return true if a connection to Tor's control interface was successfully
 * established. Otherwise, returns false and sets errmsg.
 * */
void
ControlConnection::connect(QHostAddress addr, quint16 port)
{
  /* Store the control interface address information */
  _addr = addr;
  _port = port;
  _run  = true;

  /* Start a thread. Either connected() or connectFailed() will be emitted
   * once the connection attempt has completed. */
  QThread::start();
}

/** \return true if the control socket is connected and the message handling
 * thread is running. */
bool
ControlConnection::isConnected()
{
  /* If the socket gets disconnected, the thread exits */
  return (_run && isRunning());
}

/** Disconnects the control socket and stops all message processing. */
void
ControlConnection::disconnect()
{
  /* Check if we're already disconnected */
  if (!isConnected()) {
    return;
  }
  _run = false;
 
  /* Flush any outstanding waiters in the send queue */
  _sendMutex.lock();
  while (!_sendQueue.isEmpty()) {
    SendWaiter *waiter = _sendQueue.dequeue();
    waiter->setResult(Failed, tr("The control socket is closing."));
  }
  _sendMutex.unlock();

  /* Flush any outstanding waiters in the receive queue */
  _recvMutex.lock();
  while (!_recvQueue.isEmpty()) {
    ReceiveWaiter *waiter = _recvQueue.dequeue();
    waiter->setResult(Failed, ControlReply(), 
                       tr("The control socket is closing."));
  }
  _recvMutex.unlock();
}

/** Sends a control command to Tor and waits for the response.
 * \param cmd The command to send to Tor.
 * \param reply The response from Tor.
 * \param errmsg Stores an error message if the send or receive failed.
 * \return true if the command was sent and a response was received.
 */
bool
ControlConnection::send(ControlCommand cmd, ControlReply &reply,  QString *errmsg)
{
  QWaitCondition waitCond;
  SendWaiter sendWaiter(cmd, &waitCond);
  ReceiveWaiter recvWaiter(&reply, &waitCond);
  
  /* Make sure we have a valid control socket before trying to send. */
  if (!isConnected()) {
    return err(errmsg, tr("Control socket not connected."));
  }
 
  /* Add a send waiter to the outgoing message queue */
  _sendMutex.lock();
  _sendQueue.enqueue(&sendWaiter);
    
  /* Lock the receive mutex and wait for the message to be sent. The receive
   * mutex is locked so the response doesn't get added to the incoming message
   * queue before we have a waiter for it, but we don't add a waiter yet
   * because if the send fails, then no response will ever be received. */
  _recvMutex.lock();
  waitCond.wait(&_sendMutex);
  _sendMutex.unlock();

  /* Check if the send failed. */
  if (sendWaiter.status() == Failed) {
    _recvMutex.unlock();
    return err(errmsg, sendWaiter.errorString());
  }
    
  /* The send was successful, so add a waiter for the response. */
  _recvQueue.enqueue(&recvWaiter);
  waitCond.wait(&_recvMutex);
  _recvMutex.unlock();
     
  /* If the receive failed, then get the error string */
  if (recvWaiter.status() == Failed) {
    return err(errmsg, recvWaiter.errorString());
  }
  return true;
}

/** Determines if there are any messages waiting in the outgoing message queue.
 * \return true if the send queue is empty.
 */
bool
ControlConnection::isSendQueueEmpty()
{
  QMutexLocker locker(&_sendMutex);
  return _sendQueue.isEmpty();
}

/** Processes any messages waiting in the send queue. */
void
ControlConnection::processSendQueue(ControlSocket *sock)
{
  SendWaiter *sendWaiter;
  bool result;
  QString errorString;
 
  /* Iterate through all messages waiting in the queue */
  _sendMutex.lock();
  while (!_sendQueue.isEmpty()) {
    /* Grab a waiter, try to send it, and store the result */
    sendWaiter = _sendQueue.dequeue();
    result = sock->sendCommand(sendWaiter->command(), &errorString);
    sendWaiter->setResult((result ? Success : Failed), errorString);
  }
  _sendMutex.unlock();
}

/** Processes any messages waiting on the control socket. */
void
ControlConnection::processReceiveQueue(ControlSocket *sock)
{
  ReceiveWaiter *waiter;
  
  while (sock->canReadLine()) {
    ControlReply reply;
    if (sock->readReply(reply)) {
      if (reply.getStatus() == "650") {
        /* Asynchronous event message */
        if (_events) {
          _events->handleEvent(reply);
        }
      } else {
        /* Response to a previous command */
        _recvMutex.lock();
        waiter = _recvQueue.dequeue();
        waiter->setResult(Success, reply);
        _recvMutex.unlock();
      }
    }
  }
}

/** Attempt to establish a connection to Tor's control interface. We will try
 * a maximum of MAX_CONNECT_ATTEMPTS, waiting CONNECT_RETRY_DELAY between each
 * attempt, to give slow Tors a chance to finish binding their control port. */
bool
ControlConnection::connect(ControlSocket *sock, QString *errmsg)
{
  for (int i = 0; i < MAX_CONNECT_ATTEMPTS; i++) {
    if (sock->connect(_addr, _port, errmsg)) {
      return true;
    }
    QThread::usleep(CONNECT_RETRY_DELAY);
  }
  return false;
}

/** Main thread implementation. Creates and connects a control socket and
 * processes any messages either waiting to be sent or waiting to be read off
 * the control socket, until either disconnect() is called or the socket gets
 * disconnected through some other means. */
void
ControlConnection::run()
{
  QString errmsg;
  
  /* Create a new control socket and try to connect to Tor */
  ControlSocket *sock = new ControlSocket();
  
  if (!connect(sock, &errmsg)) {
    emit connectFailed(errmsg);
  } else {
    emit connected();
    while (_run && sock->isConnected()) {
      /* If there are messages in the send queue, then send them */
      if (!isSendQueueEmpty()) {
        processSendQueue(sock);
      }
      
      /* Wait for some data to appear on the socket */
      sock->waitForReadyRead(WAIT_TIMEOUT);
      
      /* If there are messages waiting on the socket, read them in */
      if (sock->bytesAvailable()) {
        processReceiveQueue(sock);
      }
    }
    if (sock->isConnected()) {
      sock->disconnect();
    }
    emit disconnected();
  }
  _run = false;
  delete sock;
}

