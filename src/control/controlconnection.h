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
 * \file controlconnection.h
 * \version $Id$
 */

#ifndef _CONTROLCONNECTION_H
#define _CONTROLCONNECTION_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>

#include "controlsocket.h"
#include "torevents.h"

 
class ControlConnection : public QThread
{
  Q_OBJECT

public:
  /** Default constructor. */
  ControlConnection(TorEvents *events = 0);

  /** Connect to the specified Tor control interface. */
  void connect(QHostAddress addr, quint16 port);
  /** Disconnect from Tor. */
  void disconnect();
  /** Returns true if we are connected to Tor and processing messages. */
  bool isConnected();
  /** Sends a control command to Tor and waits for the response. */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);

signals:
  /** Emitted when a control connection has been established. */
  void connected();
  /** Emitted when a control connection has been closed. */
  void disconnected();
  /** Emitted when a control connection fails. */
  void connectFailed(QString errmsg);

private:
  /** Main thread implementation. */
  void run();
  /** Connections the given socket to Tor. */
  bool connect(ControlSocket *socket, QString *errmsg);
  /** Checks if there are any outoing messages waiting. */
  bool isSendQueueEmpty();
  /** Processes any messages waiting to be sent. */
  void processSendQueue(ControlSocket *sock);
  /** Processes any messages waiting on the control socket. */
  void processReceiveQueue(ControlSocket *sock);

  QHostAddress _addr; /**< Address of Tor's control interface. */
  quint16 _port;      /**< Port of Tor's control interface. */
  QMutex _sendMutex;  /**< Mutex for the send queue. */
  QMutex _recvMutex;  /**< Mutex for the receive queue. */
  /** Set to false when we are to disconnect and stop processing messages. */
  bool _run;
  /** Pointer to a previously constructed TorEvents list of event handlers */
  TorEvents* _events;

  
  /** Status of a message in either the send or receive queue */
  enum WaiterStatus {
    Waiting, /**< Message is still in the queue. */
    Failed,  /**< The send or receive operation failed. */
    Success  /**< The send or receive operation completed successfully. */
  };
 
  /** An item in the send queue. */
  class SendWaiter 
  {
    public:
      /** Create a new item for the send queue. */
      SendWaiter(ControlCommand cmd, QWaitCondition *waitCond) 
      { _command = cmd; _status = Waiting; _waitCond = waitCond; }
      /** Gets the command waiting to be sent. */
      ControlCommand command() { return _command; }
      /** Gets the status of this item in the queue. */
      WaiterStatus status() { return _status; }
      /** Returns an error message if the send failed. */
      QString errorString() { return _errmsg; }
      /** Sets the result of sending an item in the send queue. */
      void setResult(WaiterStatus status, QString errmsg = QString())
      { _status = status; _errmsg = errmsg; _waitCond->wakeAll(); }
    private:
      ControlCommand _command; /**< The command to send. */
      QWaitCondition* _waitCond; /**< Waits for a command to be sent. */
      WaiterStatus _status;    /**< The status of this queue item. */
      QString _errmsg;         /**< An error message if the send failed. */
  };
  QQueue<SendWaiter *> _sendQueue; /** Queue of commands waiting to be sent. */

  /** An item in the receive queue. */
  class ReceiveWaiter
  {
    public:
      /** Create a new item for the receive queue. */
      ReceiveWaiter(ControlReply *reply, QWaitCondition *waitCond) 
      { _reply = reply; _status = Waiting; _waitCond = waitCond; }
      /** Gets the status of this item in the queue. */
      WaiterStatus status() { return _status; }
      /** Returns an error message if the receive failed. */
      QString errorString() { return _errmsg; }
      /** Sets the result of waiting for a control response. */
      void setResult(WaiterStatus status, ControlReply reply, QString errmsg = QString())
      { _status = status; *_reply = reply; _errmsg = errmsg; _waitCond->wakeAll(); }
    private:
      ControlReply* _reply; /**< The response received */
      QWaitCondition* _waitCond; /**< Waits for a reply to be received. */
      WaiterStatus _status; /**< The status of this queue item. */
      QString _errmsg;      /**< An error message if the receive failed. */
  };
  QQueue<ReceiveWaiter *> _recvQueue; /** Queue of waiters for a response. */
};

#endif

