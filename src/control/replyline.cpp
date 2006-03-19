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
 * \file replyline.cpp
 * \version $Id$
 */

#include "replyline.h"

/** Default constructor */
ReplyLine::ReplyLine()
{
}

/** Constructor */
ReplyLine::ReplyLine(QString status, QString msg)
{
  _status = status;
  setMessage(msg);
}

/** Constructor */
ReplyLine::ReplyLine(QString status, QString msg, QString data)
{
  _status = status;
  setMessage(msg);
  appendData(data);
}

/** Set the status code for this reply line. See Tor Control Protocol V1
 * specification for a description of status codes. */
void
ReplyLine::setStatus(QString status)
{
  _status = status;
}

/** Get the status code for this reply line. */
QString
ReplyLine::getStatus()
{
  return _status;
}

/** Set the message for this reply line */
void
ReplyLine::setMessage(QString msg)
{
  _message = unescape(msg);
}

/** Get the message for this reply line */
QString
ReplyLine::getMessage()
{
  return _message;
}

/** Add some data to this line */
void
ReplyLine::appendData(QString data)
{
  _data << unescape(data);
}

/** Returns a QStringList of all data lines for this reply line */
QStringList
ReplyLine::getData()
{
  return _data;
}

/** Unescapes characters in the given string */
QString
ReplyLine::unescape(QString str)
{
  /* If the line starts with a "." and was escaped, then unescape it */
  if (str.startsWith("..")) {
    str.remove(0, 1);
  }

  /* Trim off trailing whitespace (including \r\n) */
  return str.trimmed();
}

