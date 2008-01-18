/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
 *  Copyright (C) 2008,  Matt Edman
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
 * \file debug.h
 * \version $Id$
 * \brief Helper class and methods for logging debug information
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include <QString>
#include <QtDebug>


namespace log {

class Message {
  struct Stream {
    Stream(QtMsgType t, const QString &fmt)
      : type(t), buf(fmt), ref(1) {}
    QtMsgType type; /**< Message severity level. */
    QString buf;    /**< Log message buffer. */
    int ref;        /**< Reference counter. */
  } *stream;

public:
  /** Constructs a new Message with severity <b>t</b> and the message format
   * <b>fmt</b>. */
  inline Message(QtMsgType t, const QString &fmt)
    : stream(new Stream(t, fmt)) {}
  inline Message(const Message &o)
    : stream(o.stream) { ++stream->ref; }
  virtual ~Message() {
    if (!--stream->ref) {
      stream->buf.prepend("torcontrol: ");
      qt_message_output(stream->type, qPrintable(stream->buf));
      delete stream;
    }
  }

  inline Message arg(const QString &a) 
    { stream->buf = stream->buf.arg(a); return *this; }
  inline Message arg(int a)
    { stream->buf = stream->buf.arg(a); return *this; }
};

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtDebugMsg. */
inline Message
debug(const QString &fmt)
{
  return Message(QtDebugMsg, fmt);
}

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtWarningMsg. */
inline Message
warn(const QString &fmt)
{
  return Message(QtWarningMsg, fmt);
}

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtCriticalMsg. */
inline Message
error(const QString &fmt)
{
  return Message(QtCriticalMsg, fmt);
}

/** Creates a new message using <b>fmt</b> and a severity level of
 * QtFatalMsg. */
inline Message
fatal(const QString &fmt)
{
  return Message(QtFatalMsg, fmt);
}

}

#endif

