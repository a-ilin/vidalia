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
** \file controlconnection.h
** \version $Id$
** \brief A connection to Tor's control interface, responsible for sending and
** receiving commands and events
**/

#ifndef _CONTROLCONNECTION_H
#define _CONTROLCONNECTION_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QTimer>
#include <QHostAddress>

#include "eventtype.h"
#include "controlsocket.h"
#include "torevents.h"


class ControlConnection : public QThread
{
  Q_OBJECT

public:
  /** Control connection status */
  enum Status {
    Unset,         /**< Control connection status is not yet set. */
    Disconnected,  /**< Control connection disconnected.     */
    Disconnecting, /**< Control connection is disconnecting. */
    Connecting,    /**< Control connection attempt pending.  */
    Connected      /**< Control connection established.      */
  };

  /** Default constructor. */
  ControlConnection(TorEvents *events = 0);
  /** Destructor. */
  ~ControlConnection();

  /** Connect to the specified Tor control interface. */
  void connect(QHostAddress addr, quint16 port);
  /** Cancels a pending control connection to Tor. */
  void cancelConnect();
  /** Disconnect from Tor's control interface. */
  void disconnect();
  /** Returns true if the control socket is connected to Tor. */
  bool isConnected();
  /** Returns the status of the control connection. */
  Status status();
  /** Sends a control command to Tor and waits for the reply. */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);
  /** Sends a control command to Tor and does not wait for a reply. */
  bool send(ControlCommand cmd, QString *errmsg = 0);

signals:
  /** Emitted when a control connection has been established. */
  void connected();
  /** Emitted when a control connection has been closed. */
  void disconnected();
  /** Emitted when a control connection fails. */
  void connectFailed(QString errmsg);

protected:
  /** Catches events for the control socket. */
  bool eventFilter(QObject *obj, QEvent *event);

private slots:
  /** Connects to Tor's control interface. */
  void connect();
  /** Called when there is data on the control socket. */
  void onReadyRead();
  /** Called when the control socket is connected. */
  void onConnected();
  /** Called when the control socket is disconnected. */
  void onDisconnected();
  /** Called when the control socket encounters an error. */
  void onError(QAbstractSocket::SocketError error);

private:
  /** Sets the control connection status. */
  void setStatus(Status status);
  /** Returns the string description of <b>status</b>. */
  QString statusString(Status status);
  /** Main thread implementation. */
  void run();

  ControlSocket* _sock; /**< Socket used to communicate with Tor. */
  TorEvents* _events; /**< Dispatches asynchronous events from Tor. */
  Status _status; /**< Status of the control connection. */
  QHostAddress _addr; /**< Address of Tor's control interface. */
  quint16 _port; /**< Port of Tor's control interface. */
  QMutex _connMutex; /**< Mutex around the control socket. */
  QMutex _recvMutex; /**< Mutex around the queue of ReceiveWaiters. */
  QMutex _statusMutex; /**< Mutex around the connection status value. */
  int _connectAttempt; /**< How many times we've tried to connect to Tor while
                            waiting for Tor to start. */
  QTimer* _connectTimer; /**< Timer used to delay connect attempts. */

  /** Private class used to wait for a response to a control command. */
  class ReceiveWaiter {
    public:
      /** Default constructor. */
      ReceiveWaiter() { _status = Waiting; }
      /** Waits for and gets the reply from a control command. */
      bool getResult(ControlReply *reply, QString *errmsg = 0);
      /** Sets the result and reply from a control command. */
      void setResult(bool success, ControlReply reply, 
                     QString errmsg = QString());
    private:
      /** Status of the receive waiter. */
      enum ReceiveStatus { Waiting, Failed, Success } _status;
      ControlReply _reply; /**< Reply to a previous command. */
      QMutex _mutex; /**< Mutex around the wait condition. */
      QWaitCondition _waitCond; /**< Waits for a control rpely. */
      QString _errmsg; /**< Error message if the reply fails. */
  };
  QQueue<ReceiveWaiter *> _recvQueue; /**< Objects waiting for a reply. */
  
  /** Object used to wait for the result of a send operation. */
  class SendWaiter {
    public:
      /** Default constructor. */
      SendWaiter() { _status = Waiting; }
      /** Sets the result of the send operation. */
      void setResult(bool success, QString errmsg = QString());
      /** Waits for and gets the result of the send operation. */
      bool getResult(QString *errmsg = 0);
    private:
      /** Status of the send waiter. */
      enum SenderStatus { Waiting, Failed, Success } _status;
      QMutex _mutex; /**< Mutex around the wait condition. */
      QWaitCondition _waitCond; /**< Waits for the send to complete. */
      QString _errmsg; /**< Error message if the send fails. */
  };
  
  /** Private event used to push a control command to the socket's thread */
  class SendCommandEvent : public QEvent {
    public:
      /** Constructor. */
      SendCommandEvent(ControlCommand cmd, SendWaiter *waiter = 0)
      : QEvent((QEvent::Type)CustomEventType::SendCommandEvent)
      { _cmd = cmd; _waiter = waiter; }
      /** Returns the control command to send to Tor. */
      ControlCommand command() { return _cmd; }
      /** Returns a waiter (if any) for the result of this send. */
      SendWaiter* waiter() { return _waiter; }
    private:
      ControlCommand _cmd;  /**< Command to send to Tor. */
      SendWaiter* _waiter; /**< Waiter for the result of this event. */
  };
};

#endif

