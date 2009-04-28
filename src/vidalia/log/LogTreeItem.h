/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file LogTreeItem.h
** \version $Id$ 
** \brief Item representing a single message in the message log
*/

#ifndef _LOGTREEITEM_H
#define _LOGTREEITEM_H

#include "LogEvent.h"

#include <QTreeWidgetItem>
#include <QDateTime>
#include <QString>


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

