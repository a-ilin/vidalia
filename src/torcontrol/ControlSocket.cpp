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
** \file ControlSocket.cpp
** \brief Socket used to connect to Tor's control interface
*/

#include "ControlSocket.h"
#include "SendCommandEvent.h"
#include "tcglobal.h"

#include "stringutil.h"

/** Timeout reads in 250ms. We can set this to a short value because if there
* isn't any data to read, we want to return anyway. */
#define READ_TIMEOUT  250


/** Default constructor. */
ControlSocket::ControlSocket(ControlMethod::Method method)
{
  _tcpSocket = new QTcpSocket();
  _localSocket = new QLocalSocket();
  _method = method;
  switch(_method) {
    case ControlMethod::Port:
      _socket = _tcpSocket;
      break;

    case ControlMethod::Socket:
      _socket = _localSocket;
      break;
  }

  QObject::connect(_socket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
  QObject::connect(_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
  QObject::connect(_socket, SIGNAL(connected()), this, SIGNAL(connected()));
  QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                   this, SIGNAL(error(QAbstractSocket::SocketError)));
}

/** Returns true if the control socket is connected and ready to send or
 * receive. */
bool
ControlSocket::isConnected()
{
  switch(_method) {
    case ControlMethod::Port:
      return (_tcpSocket->isValid() && _tcpSocket->state() == QAbstractSocket::ConnectedState);
      break;

    default:
    case ControlMethod::Socket:
      return (_localSocket->isValid() && _localSocket->state() == QLocalSocket::ConnectedState);
      break;
  }
}

/** Connects to address:port */
void
ControlSocket::connectToHost(const QHostAddress &address, quint16 port)
{
  _tcpSocket->connectToHost(address, port);
}

/** Disconnects from host */
void
ControlSocket::disconnectFromHost()
{
  _tcpSocket->disconnectFromHost();
}

/** Connects to a unix socket file */
void
ControlSocket::connectToServer(const QString &name)
{
  _localSocket->connectToServer(name);
}

/** Disconnects from the socket */
void
ControlSocket::disconnectFromServer()
{
  _localSocket->disconnectFromServer();
}

/** Interface to QTcpSocket::canReadLine */
bool
ControlSocket::canReadLine()
{
  return _socket->canReadLine();
}

/** Returns the string description of <b>error</b>. */
QString
ControlSocket::toString(const QAbstractSocket::SocketError error)
{
  QString str;
  switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
      str = "Connection refused by peer."; break;
    case QAbstractSocket::RemoteHostClosedError:
      str = "Remote host closed the connection."; break;
    case QAbstractSocket::HostNotFoundError:
      str = "Host address not found."; break;
    case QAbstractSocket::SocketAccessError:
      str = "Insufficient access privileges."; break;
    case QAbstractSocket::SocketResourceError:
      str = "Insufficient resources."; break;
    case QAbstractSocket::SocketTimeoutError:
      str = "Socket operation timed out."; break;
    case QAbstractSocket::DatagramTooLargeError:
      str = "Datagram size exceeded the operating system limit."; break;
    case QAbstractSocket::NetworkError:
      str = "Network error occurred."; break;
    case QAbstractSocket::AddressInUseError:
      str = "Specified address already in use."; break;
    case QAbstractSocket::SocketAddressNotAvailableError:
      str = "Specified address does not belong to the host."; break;
    case QAbstractSocket::UnsupportedSocketOperationError:
      str = "The requested operation is not supported."; break;
    default:
      str = "An unidentified error occurred."; break;
  }
  return str;
}

/** Processes custom events sent to this object (e.g. SendCommandEvents) from
 * other threads. */
void
ControlSocket::customEvent(QEvent *event)
{
  if (event->type() == QEvent::User) {
    SendCommandEvent *sce = dynamic_cast<SendCommandEvent *>(event);
    if (! sce)
      return;

    QString errmsg;
    bool result = sendCommand(sce->command(), &errmsg);
    if (sce->waiter())
      sce->waiter()->setResult(result, errmsg);
    sce->accept();
  }
}

/** Send a control command to Tor on the control socket, conforming to Tor's
 * Control Protocol V1:
 *
 *   Command = Keyword Arguments CRLF / "+" Keyword Arguments CRLF Data
 *   Keyword = 1*ALPHA
 *   Arguments = *(SP / VCHAR)
 */
bool
ControlSocket::sendCommand(ControlCommand cmd, QString *errmsg)
{
  if (!isConnected()) {
    return err(errmsg, tr("Control socket is not connected."));
  }

  /* Format the control command */
  QString strCmd = cmd.toString();
  tc::debug("Control Command: %1").arg(strCmd.trimmed());

  /* Attempt to send the command to Tor */
  if (_socket->write(strCmd.toLocal8Bit()) != strCmd.length()) {
    return err(errmsg, tr("Error sending control command. [%1]")
                                            .arg(_socket->errorString()));
  }
  switch(_method) {
    case ControlMethod::Port:
      _tcpSocket->flush();
      break;

    case ControlMethod::Socket:
      _localSocket->flush();
      break;
  }
  return true;
}


/** Read a complete reply from the control socket. Replies take the following
 * form, based on Tor's Control Protocol v1:
 *
 *    Reply = *(MidReplyLine / DataReplyLine) EndReplyLine
 *
 *    MidReplyLine = "-" ReplyLine
 *    DataReplyLine = "+" ReplyLine Data
 *    EndReplyLine = SP ReplyLine
 *    ReplyLine = StatusCode [ SP ReplyText ]  CRLF
 *    ReplyText = XXXX
 *    StatusCode = XXiX
 */
bool
ControlSocket::readReply(ControlReply &reply, QString *errmsg)
{
  QChar c;
  QString line;

  if (!isConnected()) {
    return false;
  }

  /* The implementation below is (loosely) based on the Java control library
   * from Tor */
  do {
    /* Read a line of the response */
    if (!readLine(line, errmsg)) {
      return false;
    }

    if (line.length() < 4) {
      return err(errmsg, tr("Invalid control reply. [%1]").arg(line));
    }

    /* Parse the status and message */
    ReplyLine replyLine(line.mid(0, 3), line.mid(4));
    c = line.at(3);

    /* If the reply line contains data, then parse out the data up until the
     * trailing CRLF "." CRLF */
    if (c == QChar('+') &&
        !line.startsWith("250+PROTOCOLINFO")) {
        /* XXX The second condition above is a hack to deal with Tor
         * 0.2.0.5-alpha that gives a malformed PROTOCOLINFO reply. This
         * should be removed once that version of Tor is sufficiently dead. */
      while (true) {
        if (!readLine(line, errmsg)) {
          return false;
        }
        if (line.trimmed() == ".") {
          break;
        }
        replyLine.appendData(line);
      }
    }
    reply.appendLine(replyLine);
  } while (c != QChar(' '));
  return true;
}

/** Reads line data, one chunk at a time, until a newline character is
 * encountered. */
bool
ControlSocket::readLineData(QString &line, QString *errmsg)
{
  char buffer[1024];  /* Read in 1024 byte chunks at a time */
  int bytesRecv = _socket->readLine(buffer, 1024);
  while (bytesRecv != -1) {
    line.append(QString::fromLocal8Bit(buffer, bytesRecv));
    if (buffer[bytesRecv-1] == '\n') {
      break;
    }
    bytesRecv = _socket->readLine(buffer, 1024);
  }
  if (bytesRecv == -1) {
    return err(errmsg, _socket->errorString());
  }
  return true;
}

/** Reads a line of data from the socket and returns true if successful or
 * false if an error occurred while waiting for a line of data to become
 * available. */
bool
ControlSocket::readLine(QString &line, QString *errmsg)
{
  /* Make sure we have data to read before attempting anything. Note that this
   * essentially makes our socket a blocking socket */
  while (!_socket->canReadLine()) {
    if (!isConnected()) {
      return err(errmsg, tr("Socket disconnected while attempting "
                            "to read a line of data."));
    }
    _socket->waitForReadyRead(READ_TIMEOUT);
  }
  line.clear();
  return readLineData(line, errmsg);
}
