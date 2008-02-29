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
** \file logtreewidget.cpp
** \version $Id$
** \brief Contains a collection of log messages as LogTreeItems
*/

#include <QScrollBar>

#include "logtreewidget.h"
#include "logheaderview.h"


/** Default constructor. */
LogTreeWidget::LogTreeWidget(QWidget *parent)
: QTreeWidget(parent)
{
  setHeader(new LogHeaderView(this));
  
  /* Default to always scrolling to the most recent item added */
  _scrollOnNewItem = true;
  connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
          this, SLOT(onVerticalScroll(int)));
}

/** Called when the user moves the vertical scrollbar. If the user has the
 * scrollbar at within one step of its maximum, then always scroll to new
 * items when added. Otherwise, leave the scrollbar alone since they are
 * probably looking at something in their history. */
void
LogTreeWidget::onVerticalScroll(int value)
{
  QScrollBar *scrollbar = verticalScrollBar();
  _scrollOnNewItem = (value >= (scrollbar->maximum()-scrollbar->singleStep()));
}

/** Cast a QList of QTreeWidgetItem pointers to a list of LogTreeWidget
 * pointers. There really must be a better way to do this. */
QList<LogTreeItem *>
LogTreeWidget::qlist_cast(QList<QTreeWidgetItem *> inlist)
{
  QList<LogTreeItem *> outlist;
  foreach (QTreeWidgetItem *item, inlist) {
    outlist << (LogTreeItem *)item;
  }
  return outlist;
}

/** Sorts the list of pointers to log tree items by timestamp. */
QList<LogTreeItem *>
LogTreeWidget::qlist_sort(QList<LogTreeItem *> inlist)
{
  QMap<quint32, LogTreeItem *> outlist;
  foreach (LogTreeItem *item, inlist) {
    outlist.insert(item->id(), item);
  }
  return outlist.values();
}

/** The first time the log tree is shown, we need to set the default column
 * widths. */
void
LogTreeWidget::showEvent(QShowEvent *event)
{
  static bool shown = false;
  QTreeWidget::showEvent(event);
  if (!shown) {
    /* Set the default column widths the first time this is shown */
    ((LogHeaderView *)header())->resetColumnWidths();
    /* Adjust the message column properly */
    adjustMessageColumn();
    shown = true;
  }
}

/** Clears all items from the message log and resets the counter in the status
 * bar. */
void
LogTreeWidget::clearMessages()
{
  /* Clear the messages */
  clear();
}

/** Adjusts the message column width to accomodate long messages. */
void
LogTreeWidget::adjustMessageColumn()
{
  /* Adjust the message column, based on the longest item. */
  ((LogHeaderView *)header())->resize(sizeHintForColumn(MessageColumn));
}

/** Adds a message log item. */
void
LogTreeWidget::addMessageItem(LogTreeItem *item)
{
  /* Add the new item. */
  addTopLevelItem(item);
  /* Adjust the column headers to accomodate a long message, if necesssary */
  adjustMessageColumn();
}

/** Returns a list of all currently selected items. */
QStringList
LogTreeWidget::selectedMessages()
{
  QStringList messages;
  
  /* Get all selected log items */
  QList<LogTreeItem *> items = 
    qlist_cast(selectedItems());
  
  /* Format the message items as strings and put them in a list */
  foreach (LogTreeItem *item, qlist_sort(items)) {
    messages << item->toString();
  }
  return messages;
}

/** Returns a list of all items in the tree. */
QStringList
LogTreeWidget::allMessages()
{
  QStringList messages;
  
  /* Find all items */
  QList<LogTreeItem *> items = 
    qlist_cast(findItems("*", Qt::MatchWildcard|Qt::MatchWrap, MessageColumn));
  
  /* Format the message items as strings and put them in a list */
  foreach (LogTreeItem *item, qlist_sort(items)) {
    messages << item->toString();  
  }
  return messages;
}

/** Returns the number of items currently shown. */
int
LogTreeWidget::messageCount()
{
  return topLevelItemCount();
}

/** Sets the maximum number of items in the tree. */
void
LogTreeWidget::setMaximumMessageCount(int max)
{
  while (max < messageCount()) {
    /* If the new max is less than the currently displayed number of 
     * items, then we'll get rid of some. */
    delete takeTopLevelItem(0);
  }
  _maxItemCount = max;
}

/** Deselects all currently selected items. */
void
LogTreeWidget::deselectAll()
{
  foreach(QTreeWidgetItem *item, selectedItems()) {
    setItemSelected(item, false);
  }
}

/** Adds a log item to the tree and returns a pointer to the new item. */
LogTreeItem*
LogTreeWidget::log(LogEvent::Severity type, QString message)
{
  LogTreeItem *item = new LogTreeItem(type, message);

  /* If we need to make room, then make some room */
  if (messageCount() >= _maxItemCount) {
    delete takeTopLevelItem(0);
  }
  
  /* Add the new message item and scroll to it (if necessary) */
  addMessageItem(item);
  if (_scrollOnNewItem) {
    scrollToItem(item);
  }
  return item;
}

/** Filters the message log based on the given filter. */
void
LogTreeWidget::filter(uint filter)
{
  LogTreeItem *item;
  int index = messageCount() - 1;
  int itemsShown = 0;

  while (index > -1) {
    item = (LogTreeItem *)topLevelItem(index);
    if ((itemsShown < _maxItemCount) && (filter & item->severity())) {
      itemsShown++;
    } else {
      delete takeTopLevelItem(index);
    }
    index--;
  }
}

/** Searches the log for entries that contain the given text. */
QList<LogTreeItem *>
LogTreeWidget::find(QString text, bool highlight)
{
  QList<LogTreeItem *> items = 
    qlist_cast(findItems(text, Qt::MatchContains|Qt::MatchWrap, MessageColumn));
  
  if (highlight) {
    /* Deselect all items before highlighting our search results. */
    deselectAll();
    foreach (LogTreeItem *item, items) {
      /* Highlight a matched item */
      setItemSelected(item, true);
    }
  }

  /* Return the results, sorted by timestamp */
  return qlist_sort(items);
}

