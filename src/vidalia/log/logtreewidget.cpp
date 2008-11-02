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
#include "logmessagecolumndelegate.h"


/** Default constructor. */
LogTreeWidget::LogTreeWidget(QWidget *parent)
: QTreeWidget(parent)
{
  setHeader(new LogHeaderView(this));

  /* Tor's log messages are always in English, so stop Qt from futzing with
   * the message text if we're currently using a non-English RTL layout. */
  if (layoutDirection() == Qt::RightToLeft) {
    setItemDelegateForColumn(LogTreeWidget::MessageColumn,
                             new LogMessageColumnDelegate(this));
  }

  /* Explicitly default to sorting messages chronologically */
  sortItems(LogTreeWidget::TimeColumn, Qt::AscendingOrder);

  /* Default to always scrolling to the most recent item added */
  _scrollOnNewItem = true;
  setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
  connect(verticalScrollBar(), SIGNAL(sliderReleased()),
          this, SLOT(verticalSliderReleased()));
}

/** Called when the user moves the vertical scrollbar. If the user has the
 * scrollbar at within one step of its maximum, then always scroll to new
 * items when added. Otherwise, leave the scrollbar alone since they are
 * probably looking at something in their history. */
void
LogTreeWidget::verticalSliderReleased()
{
  QScrollBar *scrollBar = verticalScrollBar();
  if (header()->sortIndicatorOrder() == Qt::AscendingOrder)
    _scrollOnNewItem = (scrollBar->value() == scrollBar->maximum());
  else
    _scrollOnNewItem = (scrollBar->value() == scrollBar->minimum());
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
    shown = true;
  }
}

/** Clears all items from the message log and resets the counter in the status
 * bar. */
void
LogTreeWidget::clearMessages()
{
  /* Clear the messages */
  _itemHistory.clear();
  clear();
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

  /* Format the message items as strings and put them in a list */
  foreach (LogTreeItem *item, _itemHistory) {
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
  while (max < messageCount() && _itemHistory.size() > 0) {
    /* If the new max is less than the currently displayed number of 
     * items, then we'll get rid of some. */
    int index = indexOfTopLevelItem(_itemHistory.takeFirst());
    if (index != -1)
      delete takeTopLevelItem(index);
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
  int oldScrollValue;
  QScrollBar *scrollBar = verticalScrollBar();
  LogTreeItem *item = new LogTreeItem(type, message);

  /* Remember the current scrollbar position */
  oldScrollValue = scrollBar->value();

  /* If we need to make room, then make some room */
  if (messageCount() >= _maxItemCount && _itemHistory.size()) {
    int index = indexOfTopLevelItem(_itemHistory.takeFirst());
    if (index != -1)
      delete takeTopLevelItem(index);
  }

  /* Add the new message item.
   * NOTE: We disable sorting, add the new item, and then re-enable sorting
   *       to force the result to be sorted immediately. Otherwise, the new
   *       message is not sorted until the message log has focus again. This
   *       is totally lame.
   */
  setSortingEnabled(false);
  addLogTreeItem(item);
  setSortingEnabled(true);

  /* The intended vertical scrolling behavior is as follows:
   *
   *   1) If the message log is sorted in chronological order, and the user
   *      previously had the vertical scroll bar at its maximum position, then
   *      reposition the vertical scroll bar to the new maximum value.
   *
   *   2) If the message log is sorted in reverse chronological order, and the
   *      user previously had the vertical scroll bar at its minimum position,
   *      then reposition the vertical scroll bar to the new minimum value
   *      (which is always just 0 anyway).
   *
   *   3) If the message log is sorted by severity level or lexicographically
   *      by log message, or if the user manually repositioned the scroll bar,
   *      then leave the vertical scroll bar at its previous position.
   */
  if (_scrollOnNewItem && sortColumn() == LogTreeWidget::TimeColumn) {
    if (header()->sortIndicatorOrder() == Qt::AscendingOrder)
      scrollBar->setValue(scrollBar->maximum());
    else
      scrollBar->setValue(scrollBar->minimum());
  } else {
    scrollBar->setValue(oldScrollValue);
  }

  return item;
}

/** Adds <b>item</b> as a top-level item in the tree. */
void
LogTreeWidget::addLogTreeItem(LogTreeItem *item)
{
  addTopLevelItem(item);
  _itemHistory.append(item);
}

/** Filters the message log based on the given filter. */
void
LogTreeWidget::filter(uint filter)
{
  int itemsShown = 0;
  for (int i = _itemHistory.size()-1; i >= 0; i--) {
    LogTreeItem *item = _itemHistory.at(i);
    if ((itemsShown < _maxItemCount) && (filter & item->severity())) {
      itemsShown++;
    } else {
      int itemIndex = indexOfTopLevelItem(item);
      if (itemIndex != -1)
        delete takeTopLevelItem(itemIndex);
      _itemHistory.removeAt(i);
    }
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
