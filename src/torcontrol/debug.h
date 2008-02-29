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
** \file debug.h
** \version $Id$
** \brief Helper class and methods for logging debug information
*/

#ifndef _DEBUG_H
#define _DEBUG_H

#include <QString>
#include <QtDebug>


namespace tc {

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

