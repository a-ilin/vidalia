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
 * \file logtreeitem.cpp
 * \version $Id: messagelog.cpp 797 2006-05-09 04:42:35Z edmanm $
 * \brief Item representing a single message in the message log
 */

#include <util/string.h>
#include "logtreeitem.h"
#include "logtreewidget.h"

/** Defines the format used for displaying the date and time of a log message.*/
#define DATETIME_FMT  "MMM dd hh:mm:ss:zzz"

/* Column index values */
#define COL_TIME    LogTreeWidget::TimeColumn
#define COL_TYPE    LogTreeWidget::TypeColumn
#define COL_MESG    LogTreeWidget::MessageColumn
#define ROLE_TYPE   Qt::UserRole

/** Vertical padding on message log rows. */
#if defined(Q_WS_MAC)
/* OS X requires less vertical padding than the rest of the world, for
 * some unknown reason. */
#define VERTICAL_PADDING  1
#else
#define VERTICAL_PADDING  10
#endif


/** Default constructor. */
LogTreeItem::LogTreeItem(LogEvent::Severity type, QString message, 
                         QDateTime timestamp)
: QTreeWidgetItem()
{
  /* Set the item's log time */
  setTimestamp(timestamp);
  /* Set the item's severity and appropriate color. */
  setSeverity(type);
  /* Set the item's message text. */
  setMessage(message);
}

/** Returns a printable string representing the fields of this item. */
QString
LogTreeItem::toString()
{
  return QString("%1 [%2] %3\n").arg(text(COL_TIME))
                                .arg(text(COL_TYPE))
                                .arg(text(COL_MESG).trimmed());
}

/** Sets the item's log time. */
void
LogTreeItem::setTimestamp(QDateTime timestamp)
{
  QString strtime = timestamp.toString(DATETIME_FMT);
  setText(COL_TIME, strtime);
  setToolTip(COL_TIME, strtime);
}

/** Sets the item's severity and the appropriate background color. */
void
LogTreeItem::setSeverity(LogEvent::Severity type)
{
  /* Change row and text color for serious warnings and errors. */
  if (type == LogEvent::Error) {
    /* Critical messages are red with white text. */
    for (int i = 0; i < 3; i++) {
      setBackgroundColor(i, Qt::red);
      setTextColor(i, Qt::white);
    }
  } else if (type == LogEvent::Warn) {
    /* Warning messages are yellow with black text. */
    for (int i = 0; i < 3; i++) {
      setBackgroundColor(i, Qt::yellow);
    }
  }
  
  setTextAlignment(COL_TYPE, Qt::AlignCenter);
  setText(COL_TYPE, LogEvent::severityToString(type));
  setData(COL_TYPE, ROLE_TYPE, (uint)type);
}

/** Sets the item's message text. */
void
LogTreeItem::setMessage(QString message)
{
  setText(COL_MESG, message);
  setToolTip(COL_MESG, string_wrap(message, 80, " ", "\r\n"));
}

/** Returns the severity associated with this log item. */
LogEvent::Severity
LogTreeItem::severity()
{
  return (LogEvent::Severity)data(COL_TYPE, ROLE_TYPE).toUInt();
}

/** Returns the timestamp for this log message. */
QDateTime
LogTreeItem::timestamp() const
{
  return QDateTime::fromString(text(COL_TIME), DATETIME_FMT);
}

/** Returns the message for this log item. */
QString
LogTreeItem::message()
{
  return text(COL_MESG);
}

