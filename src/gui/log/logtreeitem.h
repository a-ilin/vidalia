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
 * \file logtreeitem.h
 * \version $Id: messagelog.cpp 797 2006-05-09 04:42:35Z edmanm $ 
 * \brief Item representing a single message in the message log
 */

#ifndef _LOGTREEITEM_H
#define _LOGTREEITEM_H

#include <QTreeWidgetItem>
#include <QDateTime>
#include <QString>
#include <control/logevent.h>


class LogTreeItem : public QTreeWidgetItem
{
public:
  /** Default constructor. */
  LogTreeItem(LogEvent::Severity type, QString message, 
              QDateTime timestamp = QDateTime::currentDateTime());

  /** Sets the item's log time. */
  void setTimestamp(QDateTime timestamp);
  /** Sets the item's severity and appropriate background color. */
  void setSeverity(LogEvent::Severity type);
  /** Sets the item's message text. */
  void setMessage(QString message);

  /** Returns the timestamp for this log message. */
  QDateTime timestamp() const;
  /** Returns the severity associated with this log item. */
  LogEvent::Severity severity();
  /** Returns the message associated with this log item. */
  QString message();

  /** Returns a printable string representation of the item's contents.*/
  QString toString();
};

#endif

