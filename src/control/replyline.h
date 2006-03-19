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
 */

#ifndef _REPLYLINE_H
#define _REPLYLINE_H

#include <QStringList>


class ReplyLine
{
public:
  ReplyLine();
  ReplyLine(QString status, QString message);
  ReplyLine(QString status, QString message, QString data);

  /* Get/set the status for this reply line */
  void setStatus(QString status);
  QString getStatus();

  /** Get/set the message for this reply line */
  void setMessage(QString msg);
  QString getMessage();

  /* append/retrieve the data for this reply line (could be empty) */
  void appendData(QString data);
  QStringList getData();

private:
  /* Unescape characters in the supplied string */
  QString unescape(QString str);
  
  QString _status;
  QString _message;
  QStringList _data;
};

#endif

