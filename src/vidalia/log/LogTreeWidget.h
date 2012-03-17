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
** \file LogTreeWidget.h
** \brief Contains a collection of log messages as LogTreeItems
*/

#ifndef _LOGTREEWIDGET_H
#define _LOGTREEWIDGET_H

#include "LogTreeItem.h"

#include "TorControl.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QHeaderView>
#include <QShowEvent>


class LogTreeWidget : public QTreeWidget
{
  Q_OBJECT

public:
  /** Log tree column indices. */
  enum LogColumns {
    TimeColumn    = 0, /**< Timestamp column. */
    TypeColumn    = 1, /**< Message severity type column. */
    MessageColumn = 2  /**< Message text column. */
  };

  /** Default constructor. */
  LogTreeWidget(QWidget *parent = 0);

  /** Returns a list of all currently selected messages. */
  QStringList selectedMessages();
  /** Returns a list of all messages in the tree. */
  QStringList allMessages();
  /** Deselects all currently selected messages. */
  void deselectAll();

  /** Returns the number of items currently in the tree. */
  int messageCount();
  /** Sets the maximum number of items in the tree. */
  void setMaximumMessageCount(int max);
  /** Filters the log according to the specified filter. */
  void filter(uint filter);

  /** Adds a log item to the tree. */
  LogTreeItem* log(tc::Severity severity, const QString &message);

  /** Searches the log for entries that contain the given text. */
  QList<LogTreeItem *> find(QString text, bool highlight = true);

public slots:
  /** Clears all contents on the message log and resets the counter. */
  void clearMessages();

protected:
  /** Sets the default, initial column header widths. */
  void showEvent(QShowEvent *event);

private slots:
  /** Called when the user moves the vertical scroll bar. */
  void verticalSliderReleased();

private:
  /** Adds <b>item</b> as a top-level item in the tree. */
  void addLogTreeItem(LogTreeItem *item);
  /** Casts a QList of one pointer type to another. */
  QList<LogTreeItem *> qlist_cast(QList<QTreeWidgetItem *> inlist);
  /** Sortrs a QList of pointers to tree items. */
  QList<LogTreeItem *> qlist_sort(QList<LogTreeItem *> inlist);

  /**< List of pointers to all log message items currently in the tree. */
  QList<LogTreeItem *> _itemHistory;
  int _maxItemCount; /**< Maximum number of items in the tree. */
  bool _scrollOnNewItem; /**< Set to true if we are to scroll to the new item
                               after adding a message to the log. */
};

#endif

