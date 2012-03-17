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
** \file ReplyLine.cpp
** \brief Reply from a previous control command sent to Tor
*/

#include "ReplyLine.h"


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

