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
** \file ReplyLine.h
** \brief Reply from a previous control command sent to Tor
*/

#ifndef _REPLYLINE_H
#define _REPLYLINE_H

#include <QStringList>


class ReplyLine
{
public:
  ReplyLine();
  ReplyLine(const QString &status, const QString &message);
  ReplyLine(const QString &status, const QString &message, const QString &data);

  /** Set the status code to <b>status</b>. */
  void setStatus(const QString &status);
  /** Returns the status code for this reply line. */
  QString getStatus() const;

  /** Sets the ReplyText message this reply line to <b>msg</b>. */
  void setMessage(const QString &msg);
  /** Returns the ReplyText portion of this reply line. */
  QString getMessage() const;

  /** Appends <b>data</b> to this reply line. */
  void appendData(const QString &data);
  /** Returns a QStringList of all data lines for this reply line. */
  QStringList getData() const;
  /** Returns true if this reply contained a data portion. */
  bool hasData() const { return _data.size() > 0; }

  /** Returns the entire contents of this reply line, including the status,
   * message, and any extra data. */
  QString toString() const;

private:
  /** Unescapes special characters in <b>str</b> and returns the unescaped
   * result. */
  static QString unescape(const QString &escaped);

  QString _status;    /**< Response status code. */
  QString _message;   /**< ReplyText portion of this reply line. */
  QStringList _data;  /**< Contents of any DataReplyLines in this line. */
};

#endif

