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
 * \brief Reply from a previous control command sent to Tor
 */

#include "replyline.h"

/** Default constructor */
ReplyLine::ReplyLine()
{
}

/** Constructor */
ReplyLine::ReplyLine(const QString &status, const QString &msg)
{
  _status = status;
  setMessage(msg);
}

/** Constructor */
ReplyLine::ReplyLine(const QString &status, const QString &msg, 
                     const QString &data)
{
  _status = status;
  setMessage(msg);
  appendData(data);
}

/** Set the status code for this reply line. See Tor Control Protocol V1
 * specification for a description of status codes. */
void
ReplyLine::setStatus(const QString &status)
{
  _status = status;
}

/** Returns the status code for this reply line. */
QString
ReplyLine::getStatus() const
{
  return _status;
}

/** Sets the ReplyText message this reply line to <b>msg</b>. */
void
ReplyLine::setMessage(const QString &msg)
{
  _message = unescape(msg);
}

/** Returns the ReplyText portion of this reply line. */
QString
ReplyLine::getMessage() const
{
  return _message;
}

/** Appends <b>data</b> to this reply line. */
void
ReplyLine::appendData(const QString &data)
{
  _data << unescape(data);
}

/** Returns a QStringList of all data lines for this reply line */
QStringList
ReplyLine::getData() const
{
  return _data;
}

/** Unescapes special characters in <b>str</b> and returns the unescaped
 * result. */
QString
ReplyLine::unescape(const QString &escaped)
{
  QString str = escaped;
  /* If the line starts with a "." and was escaped, then unescape it */
  if (str.startsWith("..")) {
    str.remove(0, 1);
  }

  /* Trim off trailing whitespace (including \r\n) */
  return str.trimmed();
}

QString
ReplyLine::toString() const
{
  QString str = _status + " " + _message;
  if (!_data.isEmpty()) {
    str.append("\n");
    str.append(_data.join("\n"));
  }
  return str;
}

