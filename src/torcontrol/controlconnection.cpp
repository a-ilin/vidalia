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
** \file controlconnection.cpp
** \version $Id$
** \brief A connection to Tor's control interface, responsible for sending and
** receiving commands and events
*/

#include <QCoreApplication>
#include <QMutexLocker>
#include <stringutil.h>
#include "debug.h"

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
ControlConnection::connect(const QHostAddress &addr, quint16 port)
{
  if (isRunning()) {
    tc::error("Bug: Tried to call ControlConnection::connect() when the "
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
  tc::debug("Connecting to Tor (Attempt %1 of %2)").arg(_connectAttempt)
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
      tc::debug("Control connection refused. Retrying in %1ms.")
                                       .arg(CONNECT_RETRY_DELAY);
      _connectTimer->start(CONNECT_RETRY_DELAY);
    } else {
      /* Exceeded maximum number of connect attempts. Give up. */
      QString errstr = ControlSocket::toString(error);
      tc::error("Vidalia was unable to connect to Tor: %1").arg(errstr);
      emit connectFailed(tr("Vidalia was unable to connect to Tor. (%1)")
                                                             .arg(errstr));
      setStatus(Disconnected);
    }
  } else if (error == QAbstractSocket::RemoteHostClosedError) {
    /* Tor closed the connection. This is common when we send a 'shutdown' or
     * 'halt' signal to Tor and doesn't need to be logged as loudly. */
    tc::warn("Tor closed the control connection.");
  } else {
    /* Some other error. */
    /*XXX We may want to be emitting these so the GUI thread can learn about
     * them and display an error message. */
    tc::error("Control socket error: %1").arg(ControlSocket::toString(error));
  }
}

/** Cancels a pending control connection to Tor. */
void
ControlConnection::cancelConnect()
{
  tc::warn("Control connection attempt cancelled.");
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
  tc::debug("Control connection status changed from '%1' to '%2'")
                                       .arg(statusString(_status))
                                       .arg(statusString(status));
  _status = status;
}

/** Sends a control command to Tor and waits for the reply. */
bool
ControlConnection::send(const ControlCommand &cmd,
                        ControlReply &reply, QString *errmsg)
{
  ReceiveWaiter w;
  bool result = false;
  QString errstr;
  
  _connMutex.lock();
  if (_sock->sendCommand(cmd, &errstr)) {
    /* Create and enqueue a new receive waiter */
    _recvQueue.enqueue(&w);
    _connMutex.unlock();
  } else {
    _connMutex.unlock();
    tc::error("Failed to send control command (%1): %2").arg(cmd.keyword())
                                                        .arg(errstr);
    return err(errmsg, errstr);
  }

  /* Wait for and get the result, clean up, and return */
  result = w.getResult(&reply, &errstr);
  if (!result) {
    tc::error("Failed to receive control reply: %1").arg(errstr);
    if (errmsg)
      *errmsg = errstr;
  }
  return result;
}

/** Sends a control command to Tor and returns without waiting for the
 * response. */
bool
ControlConnection::send(const ControlCommand &cmd, QString *errmsg)
{
  QMutexLocker locker(&_connMutex);
  return _sock->sendCommand(cmd, errmsg);
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
        tc::debug("Control Event: %1").arg(reply.toString());
        
        if (_events) {
          _events->handleEvent(reply);
        }
      } else {
        /* Response to a previous command */
        tc::debug("Control Reply: %1").arg(reply.toString());
        
        if (!_recvQueue.isEmpty()) {
          waiter = _recvQueue.dequeue();
          waiter->setResult(true, reply);
        }
      }
    } else {
      tc::error("Unable to read control reply: %1").arg(errmsg);
    }
  }
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

  _connMutex.unlock();
  
  /* Attempt to connect to Tor */
  connect();
  tc::debug("Starting control connection event loop.");
  exec();
  tc::debug("Exited control connection event loop.");

  /* Clean up the socket */
  _connMutex.lock();
  _sock->disconnect(this);
  delete _sock;
  delete _connectTimer;
  _sock = 0;

  /* If there are any messages waiting for a response, clear them. */
  while (!_recvQueue.isEmpty()) {
    ReceiveWaiter *w = _recvQueue.dequeue();
    w->setResult(false, ControlReply(), 
                 tr("Control socket is not connected."));
  }
  _connMutex.unlock();
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
                                            const ControlReply &reply, 
                                            const QString &errmsg)
{
  _mutex.lock();
  _status = (success ? Success : Failed);
  _reply = reply; 
  _errmsg = errmsg;
  _mutex.unlock();
  _waitCond.wakeAll();
}

