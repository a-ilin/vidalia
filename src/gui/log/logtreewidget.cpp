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
 * \file logtreewidget.cpp
 * \version $Id: messagelog.cpp 797 2006-05-09 04:42:35Z edmanm $ 
 */

#include "logtreewidget.h"
#include "logheaderview.h"


/** Default constructor. */
LogTreeWidget::LogTreeWidget(QWidget *parent)
: QTreeWidget(parent)
{
  setStatusTip(tr("Messages Shown: ") + "0");

  setHeader(new LogHeaderView(this));
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

/** Returns a list of all currently selected items. */
QList<LogTreeItem *>
LogTreeWidget::selectedItems()
{
  QList<LogTreeItem *> items = 
    qlist_cast(QTreeWidget::selectedItems());
  qSort(items); /* Sort items by timestamp */
  return items;
}

/** Returns a list of all selected items as a formatted string. */
QString
LogTreeWidget::selectedItemsText()
{
  QString text;
  foreach (LogTreeItem *item, selectedItems()) {
    text.append(item->toString());
  }
  return text;
}

/** Returns a list of all items in the tree. */
QList<LogTreeItem *>
LogTreeWidget::allItems()
{
  /* Find all items */
  return find("*", false);
}

/** Returns the number of items currently shown. */
int
LogTreeWidget::itemCount()
{
  return topLevelItemCount();
}

/** Sets the maximum number of items in the tree. */
void
LogTreeWidget::setMaximumItemCount(int max)
{
  while (max < itemCount()) {
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
  foreach(LogTreeItem *item, selectedItems()) {
    setItemSelected(item, false);
  }
}

/** Adds a log item to the tree and returns a pointer to the new item. */
LogTreeItem*
LogTreeWidget::log(LogEvent::Severity type, QString message)
{
  LogTreeItem *item = new LogTreeItem(type, message);

  /* If we need to make room, then make some room */
  if (itemCount() >= _maxItemCount) {
    delete takeTopLevelItem(0);
  }
  
  /* Add the new message item */
  addTopLevelItem(item);

  /* Update our tooltip and return the new log item */
  setStatusTip(tr("Messages Shown: %1").arg(itemCount()));
  return item;
}

/** Filters the message log based on the given filter. */
void
LogTreeWidget::filter(uint filter)
{
  LogTreeItem *item;
  int index = topLevelItemCount() - 1;
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

  setStatusTip(tr("Messages Shown: %1").arg(itemCount()));
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
  qSort(items);
  return items;
}

