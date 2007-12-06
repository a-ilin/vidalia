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
 * \version $Id$ 
 * \brief Item representing a single message in the message log
 */

#ifndef _LOGTREEITEM_H
#define _LOGTREEITEM_H

#include <QTreeWidgetItem>
#include <QDateTime>
#include <QString>
#include <logevent.h>


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

  /** Returns this message's sequence number. */
  quint32 id() const { return _seqnum; }
  /** Returns the timestamp for this log message. */
  QDateTime timestamp() const;
  /** Returns the severity associated with this log item. */
  LogEvent::Severity severity() const;
  /** Returns the message associated with this log item. */
  QString message() const;
  
  /** Returns a printable string representation of the item's contents.*/
  QString toString() const;
  /** Compares <b>other</b> to this log message item based on the current sort
   * column and order. */
  virtual bool operator<(const QTreeWidgetItem &other) const;

private:
  quint32 _seqnum;  /**< Sequence number used to disambiguate messages with
                         the same timestamp. */
};

#endif

