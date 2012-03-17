/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file LogTreeItem.cpp
** \brief Item representing a single message in the message log
*/

#include "LogTreeItem.h"
#include "LogTreeWidget.h"

#include "stringutil.h"

/** Defines the format used for displaying the date and time of a log message.*/
#define DATETIME_FMT  "MMM dd hh:mm:ss.zzz"

/* Column index values */
#define COL_TIME    LogTreeWidget::TimeColumn
#define COL_TYPE    LogTreeWidget::TypeColumn
#define COL_MESG    LogTreeWidget::MessageColumn
#define ROLE_TYPE   Qt::UserRole


/** Default constructor. */
LogTreeItem::LogTreeItem(tc::Severity type, const QString &message,
                         const QDateTime &timestamp)
  : QTreeWidgetItem()
{
  static quint32 seqnum = 0;

  /* Set this message's sequence number */
  _seqnum = seqnum++;
  /* Set the item's log time */
  setTimestamp(timestamp);
  /* Set the item's severity and appropriate color. */
  setSeverity(type);
  /* Set the item's message text. */
  setMessage(message);
}

/** Returns a printable string representing the fields of this item. */
QString
LogTreeItem::toString() const
{
  return QString("%1 [%2] %3").arg(text(COL_TIME))
                              .arg(text(COL_TYPE))
                              .arg(text(COL_MESG).trimmed());
}

/** Sets the item's log time. */
void
LogTreeItem::setTimestamp(const QDateTime &timestamp)
{
  QString strtime = timestamp.toString(DATETIME_FMT);
  setText(COL_TIME, strtime);
  setToolTip(COL_TIME, strtime);
}

/** Sets the item's severity and the appropriate background color. */
void
LogTreeItem::setSeverity(tc::Severity type)
{
  /* Change row and text color for serious warnings and errors. */
  if (type == tc::ErrorSeverity) {
    /* Critical messages are red with white text. */
    for (int i = 0; i < 3; i++) {
      setBackgroundColor(i, Qt::red);
      setTextColor(i, Qt::white);
    }
  } else if (type == tc::WarnSeverity) {
    /* Warning messages are yellow with black text. */
    for (int i = 0; i < 3; i++) {
      setBackgroundColor(i, Qt::yellow);
    }
  }

  setTextAlignment(COL_TYPE, Qt::AlignCenter);
  setText(COL_TYPE, severityToString(type));
  setData(COL_TYPE, ROLE_TYPE, (uint)type);
}

/** Sets the item's message text. */
void
LogTreeItem::setMessage(const QString &message)
{
  setText(COL_MESG, message);
  setToolTip(COL_MESG, string_wrap(message, 80, " ", "\r\n"));
}

/** Returns the severity associated with this log item. */
tc::Severity
LogTreeItem::severity() const
{
  return (tc::Severity)data(COL_TYPE, ROLE_TYPE).toUInt();
}

/** Returns the timestamp for this log message. */
QDateTime
LogTreeItem::timestamp() const
{
  return QDateTime::fromString(text(COL_TIME), DATETIME_FMT);
}

/** Returns the message for this log item. */
QString
LogTreeItem::message() const
{
  return text(COL_MESG);
}

/** Converts a tc::Severity enum value to a localized string description. */
QString
LogTreeItem::severityToString(tc::Severity severity)
{
  QString str;
  switch (severity) {
    case tc::DebugSeverity:  str = tr("Debug"); break;
    case tc::InfoSeverity:   str = tr("Info"); break;
    case tc::NoticeSeverity: str = tr("Notice"); break;
    case tc::WarnSeverity:   str = tr("Warning"); break;
    case tc::ErrorSeverity:  str = tr("Error"); break;
    default: str = tr("Unknown"); break;
  }
  return str;
}

/** Compares <b>other</b> to this log message item based on the current sort
 * column. */
bool
LogTreeItem::operator<(const QTreeWidgetItem &other) const
{
  LogTreeItem *that = (LogTreeItem *)&other;
  int sortColumn = (treeWidget() ? treeWidget()->sortColumn() : COL_TIME);

  switch (sortColumn) {
    case COL_TIME:
      /* Sort chronologically */
      return (this->_seqnum < that->_seqnum);
    case COL_TYPE:
      /* Sort by severity, then chronologically */
      if (this->severity() == that->severity()) {
        return (this->_seqnum < that->_seqnum);
      }
      /* The comparison is flipped because higher severities have
       * lower numeric values */
      return (this->severity() > that->severity());
    default:
      /* Sort by message, then chronologically */
      QString thisMessage = this->message().toLower();
      QString thatMessage = that->message().toLower();

      if (thisMessage == thatMessage) {
        return (this->_seqnum < that->_seqnum);
      }
      return (thisMessage < thatMessage);
  }
  return QTreeWidgetItem::operator<(other);
}

