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
 * \file replyline.h
 * \version $Id$
 * \brief Reply from a previous control command sent to Tor
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

