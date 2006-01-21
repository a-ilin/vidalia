/****************************************************************
 *  $Id$
 * 
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
                     
#include <QHostAddress>

#include "controlconnection.h"

ControlConnection::ControlConnection()
{
}

ControlConnection::~ControlConnection()
{
}

/** Connects to Tor's control socket on the specified host and port. If the
 * connection is successful, true is returned. If the connection fails, then
 * this function returns false and sets <b>errmsg</b> appropriately, if not
 * null. */
bool
ControlConnection::connect(QHostAddress addr, quint16 port, QString *errmsg)
{
  connectToHost(addr, port);
  if (!waitForConnected(-1)) {
    if (errmsg) {
      *errmsg = 
        QString("Error connecting to %1:%2 [%3]").arg(addr.toString()).arg(port).arg(errorString());
    }
    return false;
  }
  return true;
}

/** Disconnects from Tor's control socket */
bool
ControlConnection::disconnect(QString *errmsg)
{
  disconnectFromHost();
  if (state() != QAbstractSocket::UnconnectedState) {
    if (!waitForDisconnected(-1)) {
      if (errmsg) {
        *errmsg =
          QString("Error disconnecting socket. [%1]").arg(errorString());
      }
      return false;
    }
  }
  return true;
}

/** Send a control command to Tor on the control socket, conforming to Tor's
 * Control Protocol V1:
 *
 *   Command = Keyword Arguments CRLF / "+" Keyword Arguments CRLF Data
 *   Keyword = 1*ALPHA
 *   Arguments = *(SP / VCHAR)
 */
bool
ControlConnection::sendCommand(ControlCommand cmd, QString *errmsg)
{
  if (!isValid()) {
    return false;
  }
  
  /* Format the control command */
  QString strCmd = cmd.toString();

  /* Attempt to send the command to Tor */
  if (write(strCmd.toAscii()) != strCmd.length()) {
    if (errmsg) {
      *errmsg = 
        QString("Error sending control command. [%1]").arg(errorString());
    }
    return false;
  }
  flush();
  return true;
}

/** Reads a line of data from the socket and returns true if successful or
 * false if an error occurred while waiting for a line of data to become
 * available. */
bool
ControlConnection::readLine(QString &line, QString *errmsg)
{
  /* Make sure we have data to read before attempting anything. Note that this
   * essentially makes our socket a blocking socket */
  while (!canReadLine()) {
    if (!isValid()) {
      if (errmsg) {
        *errmsg = "Socket is not connected";
      }
      return false;
    }
    waitForReadyRead(250);
  }
  line = QAbstractSocket::readLine();
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
ControlConnection::readReply(ControlReply &reply, QString *errmsg)
{
  QChar c;
  QString line;

  if (!isValid()) {
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
          QString("Invalid control reply. [%1]").arg(line);
      }
      return false;
    }

    /* Parse the status and message */
    ReplyLine replyLine(line.mid(0, 3), line.mid(4));
    c = line.at(3);

    /* If the reply line contains data, then parse out the data up until the
     * trailing CRLF "." CRLF */
    if (c == QChar('+')) {
      QString data;
      while (true) {
        readLine(line);
        if (line == ".") {
          break;
        }
        replyLine.appendData(line);
      }
    }
    reply.appendLine(replyLine);
  } while (c != QChar(' '));
  return true;
}

/** Sends a control command and immediately waits for the control reply */
bool
ControlConnection::send(ControlCommand cmd, 
                        ControlReply &reply, QString *errmsg)
{
  if (sendCommand(cmd, errmsg)) {
    if (readReply(reply, errmsg)) {
      return true;
    }
  }
  return false;
}

