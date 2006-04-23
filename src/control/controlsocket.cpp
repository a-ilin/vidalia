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
 * \file controlsocket.cpp
 * \version $Id: $
 */

#include <QHostAddress>
#include <util/string.h>

#include "controlsocket.h"


/** Default constructor. */
ControlSocket::ControlSocket()
{
}

/** Connects to Tor's control socket on the specified host and port. If the
 * connection is successful, true is returned. If the connection fails, then
 * this function returns false and sets <b>errmsg</b> appropriately, if not
 * null. */
bool
ControlSocket::connect(QHostAddress addr, quint16 port, QString *errmsg)
{
  ProtocolVersion version;

  /* Connect the control socket. */
  connectToHost(addr, port);
  if (!waitForConnected(CONN_TIMEOUT)) {
    if (errmsg) {
      *errmsg = tr("Error connecting to %1:%2 [%3]")
                                            .arg(addr.toString())
                                            .arg(port)
                                            .arg(errorString());
    }
    return false;
  }
  
  /* Verify that Tor is speaking a protocol version we understand. */
  version = protocolVersion();
  if (version != Version1) {
    if (errmsg) {
      *errmsg =
        tr("Vidalia only supports Version 1 of Tor's Control Protocol "
           "(Version %1 detected).\n"
           "Upgrade to a newer version of Tor.").arg(version);
    }
    disconnect();
    return false;
  }

  /* Ok, now we're really connected */
  return true;
}

/** Disconnects from Tor's control socket */
bool
ControlSocket::disconnect(QString *errmsg)
{
  disconnectFromHost();
  if (isConnected()) {
    if (!waitForDisconnected(CONN_TIMEOUT)) {
      if (errmsg) {
        *errmsg =
          tr("Error disconnecting socket. [%1]").arg(errorString());
      }
      return false;
    }
  }
  return true;
}

/** Returns true if the control socket is connected and ready to send or
 * receive. */
bool
ControlSocket::isConnected()
{
  return (isValid() && state() == QAbstractSocket::ConnectedState);
}

/** Determines which version of Tor's control protocol is being spoken. */
ControlSocket::ProtocolVersion
ControlSocket::protocolVersion()
{
  QByteArray versionData;

  /* Send a special little bit of data and wait for the response */
  if (!write("\0\0\r\n", 4)) {
    return VersionUnknown;
  }
  while (bytesAvailable() < 4) {
    if (!waitForReadyRead(-1)) {
      return VersionUnknown;
    }
  }

  /* If the response starts with a "\0\0", that means it is V0 of the control
   * protocol. Otherwise, it is V1. */
  versionData = readAll();
  return (!qstrlen(versionData.data()) ? Version0 : Version1);
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
    return false;
  }
  
  /* Format the control command */
  QString strCmd = cmd.toString();

  /* Attempt to send the command to Tor */
  if (write(strCmd.toAscii()) != strCmd.length()) {
    if (errmsg) {
      *errmsg = 
        tr("Error sending control command. [%1]").arg(errorString());
    }
    return false;
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
      if (errmsg) {
        *errmsg = tr("Socket disconnected while attempting to read a line of data.");
      }
      return false;
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
      if (errmsg) {
        *errmsg = 
          tr("Invalid control reply. [%1]").arg(line);
      }
      return false;
    }

    /* Parse the status and message */
    ReplyLine replyLine(line.mid(0, 3), line.mid(4));
    c = line.at(3);

    /* If the reply line contains data, then parse out the data up until the
     * trailing CRLF "." CRLF */
    if (c == QChar('+')) {
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

