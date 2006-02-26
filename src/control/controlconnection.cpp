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

#include "controlconnection.h"


/** Maximum time to wait for data to arrive on the socket. */
#define WAIT_TIMEOUT    10


/** Constructor.
 * \param events a TorEvents object used to dispatch asynchronous events
 * received from Tor.
 */
ControlConnection::ControlConnection(TorEvents *events)
{
  _events = events;
}

/** Destructor. */
ControlConnection::~ControlConnection()
{
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
bool
ControlConnection::connect(QHostAddress addr, quint16 port, QString *errmsg)
{
  bool connected;
  
  /* Store the control interface address information */
  _addr = addr;
  _port = port;
  _run  = true;

  /* Start a thread and wait for it to finish its connection attempt */
  QThread::start();
  _connMutex.lock();
  _connWait.wait(&_connMutex);
  connected = _connected;
  _connMutex.unlock();

  /* If an error occurred, return the reason if the caller wants it */
  if (!connected) {
    if (errmsg) {
      *errmsg = _errorString;
    }
  }
  return connected;
}

/** \return true if the control socket is connected and the message handling
 * thread is running. */
bool
ControlConnection::isConnected()
{
  return (_connected && _run && isRunning());
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
  while (!_sendMutex.tryLock()) {
    QCoreApplication::processEvents();
  }
  while (!_sendQueue.isEmpty()) {
    SendWaiter *waiter = _sendQueue.dequeue();
    waiter->setResult(Failed, "The control socket is closing.");
  }
  _sendMutex.unlock();

  /* Flush any outstanding waiters in the receive queue */
  while (!_recvMutex.tryLock()) {
    QCoreApplication::processEvents();
  }
  while (!_recvQueue.isEmpty()) {
    ReceiveWaiter *waiter = _recvQueue.dequeue();
    waiter->setResult(Failed, ControlReply(), "The control socket is closing.");
  }
  _recvMutex.unlock();

  /* Wait for the socket to disconnect and thread to finish */
  while (_connected) {
    QCoreApplication::processEvents();
  }
}

/** Sends a control command to Tor.
 * \param cmd The command to send.
 * \return true if the command was sent successfully.
 */
bool
ControlConnection::sendCommand(ControlCommand cmd, QString *errmsg)
{
  SendWaiter waiter(cmd);

  /* Place a message in the send queue */
  _sendMutex.lock();
  _sendQueue.enqueue(&waiter);
  _sendMutex.unlock();
  
  /* Wait for the message to be sent */
  while (waiter.status() == Waiting) {
    /* Allow the gui to remain responsive while waiting */
    QCoreApplication::processEvents();
  }
  
  /* If the send failed, then get the error string */
  if (waiter.status() == Failed) {
    if (errmsg) {
      *errmsg = waiter.errorString();
    }
  }
  return (waiter.status() == Success);
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
  ReceiveWaiter waiter(&reply);
  
  /* Make sure we have a valid control socket before trying to send. */
  if (!isConnected()) {
    if (errmsg) {
      *errmsg = "Control socket not connected.";
    }
  } else {  
    _recvMutex.lock();
    /* Try to send the command to Tor */
    if (sendCommand(cmd, errmsg)) {
      /* Place a waiter in the receive queue */
      _recvQueue.enqueue(&waiter);
      _recvMutex.unlock();
 
      while (waiter.status() == Waiting) {
        /* Allow the gui to remain responsive while waiting */
        QCoreApplication::processEvents();
      }

      /* If the send failed, then get the error string */
      if (waiter.status() == Failed) {
        if (errmsg) {
          *errmsg = waiter.errorString();
        }
      }
      return (waiter.status() == Success);
    }
    _recvMutex.unlock();
  }
  return false;
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
ControlConnection::processSendQueue()
{
  SendWaiter *sendWaiter;
  bool result;
  QString errorString;
 
  /* Iterate through all messages waiting in the queue */
  _sendMutex.lock();
  while (!_sendQueue.isEmpty()) {
    /* Grab a waiter, try to send it, and store the result */
    sendWaiter = _sendQueue.dequeue();
    result = _sock->sendCommand(sendWaiter->command(), &errorString);
    sendWaiter->setResult((result ? Success : Failed), errorString);
  }
  _sendMutex.unlock();
}

/** Processes any messages waiting on the control socket. */
void
ControlConnection::processReceiveQueue()
{
  ReceiveWaiter *waiter;
  
  while (_sock->canReadLine()) {
    ControlReply reply;
    if (_sock->readReply(reply)) {
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

/** Main thread implementation. Creates and connects a control socket and
 * processes any messages either waiting to be sent or waiting to be read off
 * the control socket, until either disconnect() is called or the socket gets
 * disconnected through some other means. */
void
ControlConnection::run()
{
  /* Create a new control socket and try to connect to Tor */
  _sock = new ControlSocket(); 
  _connected = _sock->connect(_addr, _port, &_errorString);
  _connWait.wakeAll();

  if (_connected) {
    emit connected();
    while (_run && _sock->isConnected()) {
      /* If there are messages in the send queue, then send them */
      if (!isSendQueueEmpty()) {
        processSendQueue();
      }
      
      /* Wait for some data to appear on the socket */
      _sock->waitForReadyRead(WAIT_TIMEOUT);
      
      /* If there are messages waiting on the socket, read them in */
      if (_sock->bytesAvailable()) {
        processReceiveQueue();
      }
    }
    if (_sock->isConnected()) {
      _sock->disconnect();
    }
    emit disconnected();
  }
  _connected = _run = false;
  delete _sock;
}

