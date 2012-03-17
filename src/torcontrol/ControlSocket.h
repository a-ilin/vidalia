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
** \file ControlSocket.h
** \brief Socket used to connect to Tor's control interface
*/

#ifndef _CONTROLSOCKET_H
#define _CONTROLSOCKET_H

#include "ControlCommand.h"
#include "ControlReply.h"
#include "ControlMethod.h"

#include <QtCore>
#include <QLocalSocket>
#include <QTcpSocket>

class ControlSocket : public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  ControlSocket(ControlMethod::Method method = ControlMethod::Port);

  /** Send a command to Tor */
  bool sendCommand(ControlCommand cmd, QString *errmsg = 0);
  /** Read a response from Tor */
  bool readReply(ControlReply &reply, QString *errmsg = 0);

  /** Returns true if the control socket is connected and ready to send or
   * receive. */
  bool isConnected();
  /** Interface to each socket's canReadLine implementation */
  bool canReadLine();

  void connectToHost(const QHostAddress &address, quint16 port);
  /** Disconnects from host */
  void disconnectFromHost();
  /** Connects to a unix socket file */
  void connectToServer(const QString &name);
  /** Disconnects from the socket */
  void disconnectFromServer();

  ControlMethod::Method getMethod() { return _method; }

  /** Returns the string description of <b>error</b>. */
  static QString toString(const QAbstractSocket::SocketError error);

signals:
  /** Interface to the signals from each socket used */
  void readyRead();
  void disconnected();
  void connected();
  void error(QAbstractSocket::SocketError);

protected:
  /** Processes custom events sent to this object (e.g. SendCommandEvents)
   * from other threads. */
  void customEvent(QEvent *event);
  /** Reads line data off the socket in chunks. */
  bool readLineData(QString &line, QString *errmsg = 0);
  /** Reads a line of data from the socket (blocking) */
  bool readLine(QString &line, QString *errmsg = 0);

private:
  QTcpSocket *_tcpSocket; /**< Socket used in the connection */
  QLocalSocket *_localSocket; /**< Socket used in the connection */
  QIODevice *_socket; /**< Abstract pointer to transparently use both sockets */
  ControlMethod::Method _method;
};

#endif

