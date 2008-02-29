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
** \file controlsocket.cpp
** \version $Id$
** \brief Socket used to connect to Tor's control interface
*/

#include <stringutil.h>
#include "debug.h"

#include "controlsocket.h"

/** Timeout reads in 250ms. We can set this to a short value because if there
* isn't any data to read, we want to return anyway. */
#define READ_TIMEOUT  250


/** Default constructor. */
ControlSocket::ControlSocket()
{
}

/** Returns true if the control socket is connected and ready to send or
 * receive. */
bool
ControlSocket::isConnected()
{
  return (isValid() && state() == QAbstractSocket::ConnectedState);
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
  if (write(strCmd.toAscii()) != strCmd.length()) {
    return err(errmsg, tr("Error sending control command. [%1]")
                                            .arg(errorString()));
  }
  flush();
  return true;
}

/** Reads line data, one chunk at a time, until a newline character is
 * encountered. */
bool
ControlSocket::readLineData(QString &line, QString *errmsg)
{
  char buffer[1024];  /* Read in 1024 byte chunks at a time */
  int bytesRecv = QAbstractSocket::readLine(buffer, 1024);
  while (bytesRecv != -1) {
    line.append(buffer);
    if (buffer[bytesRecv-1] == '\n') {
      break;
    }
    bytesRecv = QAbstractSocket::readLine(buffer, 1024);
  }
  if (bytesRecv == -1) {
    return err(errmsg, errorString());
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
  while (!canReadLine()) {
    if (!isConnected()) {
      return err(errmsg, tr("Socket disconnected while attempting "
                            "to read a line of data."));
    }
    waitForReadyRead(READ_TIMEOUT);
  }
  line.clear();
  return readLineData(line, errmsg);
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

  /* The implementation below is (loosely) based on the Java control library from Tor */
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

/** Returns the string description of <b>error</b>. */
QString
ControlSocket::toString(const QAbstractSocket::SocketError error)
{
  QString str;
  switch (error) {
    case ConnectionRefusedError:
      str = "Connection refused by peer."; break;
    case RemoteHostClosedError:
      str = "Remote host closed the connection."; break;
    case HostNotFoundError:
      str = "Host address not found."; break;
    case SocketAccessError:
      str = "Insufficient access privileges."; break;
    case SocketResourceError:
      str = "Insufficient resources."; break;
    case SocketTimeoutError:
      str = "Socket operation timed out."; break;
    case DatagramTooLargeError:
      str = "Datagram size exceeded the operating system limit."; break;
    case NetworkError:
      str = "Network error occurred."; break;
    case AddressInUseError:
      str = "Specified address already in use."; break;
    case SocketAddressNotAvailableError:
      str = "Specified address does not belong to the host."; break;
    case UnsupportedSocketOperationError:
      str = "The requested operation is not supported."; break;
    default:
      str = "An unidentified error occurred."; break;
  }
  return str;
}

