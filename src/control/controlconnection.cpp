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
 * \brief A connection to Tor's control interface, responsible for sending and
 * receiving commands and events
 */

#include <QMutexLocker>
#include <util/string.h>
#include <vidalia.h>

#include "controlconnection.h"

/** Maximum number of times we'll try to connect to Tor before giving up.*/
#define MAX_CONNECT_ATTEMPTS  5
/** Time to wait between control connection attempts (in milliseconds). */
#define CONNECT_RETRY_DELAY   2*1000


/** Default constructor. */
ControlConnection::ControlConnection(TorEvents *events)
{
  _events = events;
  _status = Unset;
}

/** Destructor. */
ControlConnection::~ControlConnection()
{
  /* Exit the event loop */
  exit();
  /* Wait for the thread to finish. */
  wait();
}

/** Connect to the specified Tor control interface. */
void
ControlConnection::connect(QHostAddress addr, quint16 port)
{
  if (isRunning()) {
    vError("Bug: Tried to call ControlConnection::connect() when the "
          "control thread is already running.");
    return;
  }

  /* Save the destination information */
  _addr = addr;
  _port = port;
  _sock = 0;
  _connectAttempt = 0;
  setStatus(Connecting);

  /* Kick off the thread in which the control socket will live */
  QThread::start();
}

/** Attempt to establish a connection to Tor's control interface. We will try
 * a maximum of MAX_CONNECT_ATTEMPTS, waiting CONNECT_RETRY_DELAY between each
 * attempt, to give slow Tors a chance to finish binding their control port. */
void
ControlConnection::connect()
{
  _connectAttempt++;
  vInfo("Connecting to Tor (Attempt %1 of %2)").arg(_connectAttempt)
                                               .arg(MAX_CONNECT_ATTEMPTS);
  _connMutex.lock();
  _sock->connectToHost(_addr, _port);
  _connMutex.unlock();
}

/** Disconnect from Tor's control interface. */
void
ControlConnection::disconnect()
{
  setStatus(Disconnecting);
  _connMutex.lock();
  _sock->disconnectFromHost();
  _connMutex.unlock();
}

/** Called when the control socket is connected. This method checks that the
 * control protocol version of the Tor we connected to is at least V1. */
void
ControlConnection::onConnected()
{
  setStatus(Connected);
  emit connected();
}

/** Called when the control socket is disconnected and stops the control
 * thread's event loop. */
void
ControlConnection::onDisconnected()
{
  setStatus(Disconnected);
  emit disconnected();
  exit(0);
}

/** Called when the control socket encounters <b>error</b>. */
void
ControlConnection::onError(QAbstractSocket::SocketError error)
{
  if (status() == Connecting) {
    /* If we got a 'connection refused' and we haven't exceeded
     * MAX_CONNECT_ATTEMPTS, then try to reconnect since Tor is probably
     * running, but it doesn't have a ControlPort open yet. */
    if (error == QAbstractSocket::ConnectionRefusedError &&
        _connectAttempt < MAX_CONNECT_ATTEMPTS) {
      vInfo("Control connection refused. Retrying in %1ms.")
                                                  .arg(CONNECT_RETRY_DELAY);
      _connectTimer->start(CONNECT_RETRY_DELAY);
    } else {
      /* Exceeded maximum number of connect attempts. Give up. */
      QString errstr = ControlSocket::toString(error);
      vWarn("Vidalia was unable to connect to Tor: %1").arg(errstr);
      emit connectFailed(tr("Vidalia was unable to connect to Tor. (%1)")
                                                             .arg(errstr));
      setStatus(Disconnected);
    }
  } else if (error == QAbstractSocket::RemoteHostClosedError) {
    /* Tor closed the connection. This is common when we send a 'shutdown' or
     * 'halt' signal to Tor and doesn't need to be logged as loudly. */
    vNotice("Tor closed the control connection.");
  } else {
    /* Some other error. */
    /*XXX We may want to be emitting these so the GUI thread can learn about
     * them and display an error message. */
    vWarn("Control socket error: %1").arg(ControlSocket::toString(error));
  }
}

/** Cancels a pending control connection to Tor. */
void
ControlConnection::cancelConnect()
{
  vNotice("Control connection attempt cancelled.");
  setStatus(Disconnected);
  exit(0);
}

/** Returns true if the control socket is connected to Tor. */
bool
ControlConnection::isConnected()
{
  return (status() == Connected);
}

/** Returns the status of the control connection. */
ControlConnection::Status
ControlConnection::status()
{
  QMutexLocker locker(&_statusMutex);
  return _status;
}

/** Returns a string description of the control Status value
 * <b>status</b>. */
QString
ControlConnection::statusString(Status status)
{
  QString str;
  switch (status) {
    case Unset:  str = "Unset"; break;
    case Disconnected:  str = "Disconnected"; break;
    case Disconnecting: str = "Disconnecting"; break;
    case Connecting: str = "Connecting"; break;
    case Connected: str = "Connected"; break;
    default:  str = "unknown";
  }
  return str;
}

/** Sets the control connection status. */
void
ControlConnection::setStatus(Status status)
{
  QMutexLocker locker(&_statusMutex);
  vNotice("Control connection status changed from '%1' to '%2'")
                                    .arg(statusString(_status))
                                    .arg(statusString(status));
  _status = status;
}

/** Sends a control command to Tor and waits for the reply. */
bool
ControlConnection::send(ControlCommand cmd, ControlReply &reply, QString *errmsg)
{
  bool result = false;
  QString errstr;

  _recvMutex.lock();
  if (send(cmd, &errstr)) {
    /* Create and enqueue a new receive waiter */
    ReceiveWaiter *w = new ReceiveWaiter();
    _recvQueue.enqueue(w);
    _recvMutex.unlock();

    /* Wait for and get the result, clean up, and return */
    result = w->getResult(&reply, &errstr);
    if (!result)
      vWarn("Failed to receive control reply: %1").arg(errstr);
    delete w;
  } else {
    vWarn("Failed to send control command (%1): %2")
      .arg(cmd.keyword()).arg(errstr);
    _recvMutex.unlock();
  }

  if (!result && errmsg)
    *errmsg = errstr;
  return result;
}

/** Sends a control command to Tor. If the current thread is not the thread
 * that actually owns the socket, the command will be pushed to the correct
 * thread before sending. */
bool
ControlConnection::send(ControlCommand cmd, QString *errmsg)
{
  QThread *socketThread;
  bool result;

  /* Check for a valid and connected socket */
  _connMutex.lock();
  if (!_sock || _status != Connected) {
    vDebug("Unable to send control command '%1' when socket status is '%2'")
                                            .arg(cmd.keyword()).arg(_status);
    _connMutex.unlock();
    return err(errmsg, tr("Control socket is not connected."));
  }
  socketThread = _sock->thread();
  _connMutex.unlock();

  if (socketThread != QThread::currentThread()) {
    /* Push the message to the correct thread before sending */
    SendWaiter *w = new SendWaiter();
    Vidalia::postEvent(_sock, new SendCommandEvent(cmd, w));
  
    /* Wait for the result, clean up, and return */
    result = w->getResult(errmsg);
    delete w;
  } else {
    /* Send the command directly on the socket */
    _connMutex.lock();
    result = _sock->sendCommand(cmd, errmsg);
    _connMutex.unlock();
  }
  return result;
}

/** Called when there is data on the control socket. */
void
ControlConnection::onReadyRead()
{
  QMutexLocker locker(&_connMutex);
  ReceiveWaiter *waiter;
  QString errmsg;
 
  while (_sock->canReadLine()) {
    ControlReply reply;
    if (_sock->readReply(reply, &errmsg)) {
      if (reply.getStatus() == "650") {
        /* Asynchronous event message */
        vDebug("Control Event: %1").arg(reply.toString());
        
        if (_events) {
          _events->handleEvent(reply);
        }
      } else {
        /* Response to a previous command */
        vInfo("Control Reply: %1").arg(reply.toString());
        
        _recvMutex.lock();
        if (!_recvQueue.isEmpty()) {
          waiter = _recvQueue.dequeue();
          waiter->setResult(true, reply);
        }
        _recvMutex.unlock();
      }
    } else {
      vWarn("Unable to read control reply: %1").arg(errmsg);
    }
  }
}

/** Catches events for the control socket. */
bool
ControlConnection::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == CustomEventType::SendCommandEvent) {
    /* Get the control command and waiter from the event */
    SendCommandEvent *sce = (SendCommandEvent *)event;
    SendWaiter *w = sce->waiter();
    QString errmsg;
    bool result;
    
    /* Send the command, if the socket exists. */
    _connMutex.lock();
    if (_sock) {
      result = _sock->sendCommand(sce->command(), &errmsg);
    } else {
      result = false;
      errmsg = tr("Control socket is not connected");
    }
    _connMutex.unlock();
    
    /* If there is someone waiting for a result, give them one. */
    if (w) {
      w->setResult(result, errmsg);
    }

    /* Stop processing this event */
    sce->accept();
    return true;
  }
  /* Pass other events on */
  return QObject::eventFilter(obj, event);
}

/** Main thread implementation. Creates and connects a control socket, then
 * spins up an event loop. */
void
ControlConnection::run()
{
  /* Create a new control socket */
  _connMutex.lock();
  _sock = new ControlSocket();
  _connectTimer = new QTimer();
  _connectTimer->setSingleShot(true);
  
  QObject::connect(_sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()),
                   Qt::DirectConnection);
  QObject::connect(_sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()),
                   Qt::DirectConnection);
  QObject::connect(_sock, SIGNAL(connected()), this, SLOT(onConnected()),
                   Qt::DirectConnection);
  QObject::connect(_sock, SIGNAL(error(QAbstractSocket::SocketError)), 
                   this, SLOT(onError(QAbstractSocket::SocketError)),
                   Qt::DirectConnection);
  QObject::connect(_connectTimer, SIGNAL(timeout()), this, SLOT(connect()),
                   Qt::DirectConnection);

  _sock->installEventFilter(this);
  _connMutex.unlock();
  
  /* Attempt to connect to Tor */
  connect();
  vDebug("Starting control connection event loop.");
  exec();
  vDebug("Exited control connection event loop.");

  /* Clean up the socket */
  _connMutex.lock();
  _sock->removeEventFilter(this);
  _sock->disconnect(this);
  delete _sock;
  delete _connectTimer;
  _sock = 0;
  _connMutex.unlock();

  /* If there are any messages waiting for a response, clear them. */
  _recvMutex.lock();
  while (!_recvQueue.isEmpty()) {
    ReceiveWaiter *w = _recvQueue.dequeue();
    w->setResult(false, ControlReply(), 
                 tr("Control socket is not connected."));
  }
  _recvMutex.unlock();
}


/*
 * ControlConnection::ReceiveWaiter
 */
/** Waits for and gets the reply from a control command. */
bool 
ControlConnection::ReceiveWaiter::getResult(ControlReply *reply, 
                                            QString *errmsg)
{
  forever {
    _mutex.lock();
    if (_status == Waiting) {
      _waitCond.wait(&_mutex);
      _mutex.unlock();
    } else {
      _mutex.unlock();
      break;
    }
  }
  if (errmsg) {
    *errmsg = _errmsg;
  }
  *reply = _reply;
  return (_status == Success);
}

/** Sets the result and reply from a control command. */
void 
ControlConnection::ReceiveWaiter::setResult(bool success, 
                                            ControlReply reply, 
                                            QString errmsg)
{
  _mutex.lock();
  _status = (success ? Success : Failed);
  _reply = reply; 
  _errmsg = errmsg;
  _mutex.unlock();
  _waitCond.wakeAll();
}


/*
 * ControlConnection::SendWaiter
 */
/** Sets the result of the send operation. */
void
ControlConnection::SendWaiter::setResult(bool success, QString errmsg)
{
  _mutex.lock();
  _status = (success ? Success : Failed);
  _errmsg = errmsg;
  _mutex.unlock();
  _waitCond.wakeAll();
}

/** Waits for and gets the result of the send operation. */
bool 
ControlConnection::SendWaiter::getResult(QString *errmsg)
{
  forever {
    _mutex.lock();
    if (_status == Waiting) {
      _waitCond.wait(&_mutex);
      _mutex.unlock();
    } else {
      _mutex.unlock();
      break;
    }
  }
  if (errmsg) {
    *errmsg = _errmsg;
  }
  return (_status == Success);
}

