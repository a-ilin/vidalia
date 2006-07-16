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
 * \file routerlistwidget.cpp
 * \version $Id$
 */

#include <QHeaderView>
#include <vidalia.h>

#include "routerlistwidget.h"


RouterListWidget::RouterListWidget(QWidget *parent)
: QTreeWidget(parent)
{
  /* Create and initialize columns */
  setHeaderLabels(QStringList() << tr("Status") << tr("Router"));

  /* Set the column size and sort order for the router list. */
  sortByColumn(NameColumn);

  /* Find out when the selected item has changed. */
  connect(this, SIGNAL(itemSelectionChanged()), 
          this, SLOT(onSelectionChanged()));
}

/** Deselects all currently selected routers. */
void
RouterListWidget::deselectAll()
{
  QList<QTreeWidgetItem *> selected = selectedItems();
  foreach (QTreeWidgetItem *item, selected) {
    setItemSelected(item, false);
  }
}

/** Inserts a new router list item into the list, in its proper sorted place
 * according to the current sort column. */
void
RouterListWidget::insertSorted(RouterListItem *item)
{
  Qt::SortOrder order = header()->sortIndicatorOrder();
  int left  = 0;
  int right = topLevelItemCount();
  int mid;

  while (left < right) {
    mid = (left + right)/2;
    if (order == Qt::AscendingOrder) {
      if (*((RouterListItem *)topLevelItem(mid)) < *item) {
        left = mid + 1;
      } else {
        right = mid;
      }
    } else {
      if (*item < *((RouterListItem *)topLevelItem(mid))) {
        left = mid+1;
      } else {
        right = mid;
      }
    }
  }
 
  if (left == topLevelItemCount()) {
    addTopLevelItem(item);
  } else {
    insertTopLevelItem(left, item);
  }
}

/** Called when the user selects a router from the list. This will search the
 * list for a router whose names starts with the key pressed. */
void
RouterListWidget::keyPressEvent(QKeyEvent *event)
{
  static QString prevKey;
  static int prevIndex;
  
  QString key = event->text();
  if (!key.isEmpty() && key.at(0).isLetterOrNumber()) {
    /* A text key was pressed, so search for routers that begin with that key. */
    QString key = event->text();
    QList<QTreeWidgetItem *> list = findItems(QString("^[%1%2].*$")
                                                  .arg(key.toUpper())
                                                  .arg(key.toLower()),
                                               Qt::MatchRegExp|Qt::MatchWrap,
                                               NameColumn);
    if (list.size() > 0) {
      /* A match was found, so deselect any previously selected routers,
       * select the new match, and make sure it's visible. */
      deselectAll();
      if (key.toLower() == prevKey.toLower()) {
        /* User pressed the same search key twice in a row, so go to the next
         * result. */
        prevIndex = (prevIndex + 1) % list.size();
      } else {
        /* New search key. Save it and show the first result in the list. */
        prevKey   = key;
        prevIndex = 0;
      }
      
      /* Select the item and scroll to it */
      setItemSelected(list.at(prevIndex), true);
      scrollToItem(list.at(prevIndex));
    }
    event->accept();
  } else {
    /* It was something we don't understand, so hand it to the parent class */
    QTreeWidget::keyPressEvent(event);
  }
}

/** Finds the list item for the given descriptor. Returns 0 if not found. */
RouterListItem*
RouterListWidget::findItem(RouterDescriptor rd)
{
  QList<QTreeWidgetItem *> list = findItems(rd.name(), 
                                            Qt::MatchExactly, 
                                            NameColumn);

  /* Many routers can have the same name, so find a match on the ID. */
  foreach (QTreeWidgetItem *item, list) {
    RouterListItem *ri = (RouterListItem *)item;
    if (ri->id() == rd.id()) {
      return ri;
    }
  }
  return 0;
}

/** Finds the list item for the given router name. Returns 0 if not found.*/
RouterListItem*
RouterListWidget::findRouterItem(QString router)
{
  QList<QTreeWidgetItem *> list = findItems(router,
                                            Qt::MatchExactly,
                                            NameColumn);
  /* It's possible that more than one router could have the same name, but
   * without a fingerprint on which to match, we just have to pick one. We'll
   * return the first match. */
  if (list.size() > 0) {
    return (RouterListItem *)list.at(0);
  }
  return 0;
}

/** Finds the list item or items for the given router IP. */
QList<RouterListItem *>
RouterListWidget::findRouterItems(QHostAddress ip)
{
  RouterListItem *item;
  QList<RouterListItem *> items;
  
  int itemCount = topLevelItemCount();
  for (int i = 0; i < itemCount; i++) {
    item = (RouterListItem *)topLevelItem(i);
    if (ip.toString() == item->descriptor().ip()) {
      items << item;
    }
  }
  return items;
}

/** Adds a router descriptor to the list. */
void
RouterListWidget::addRouter(RouterDescriptor rd)
{
  if (!rd.name().isEmpty()) {
    RouterListItem *item = findItem(rd);
    if (item) {
      /* This is an updated descriptor, so remove the old item and we'll 
       * add a new, updated item. */
      delete takeTopLevelItem(indexOfTopLevelItem(item));
    }

    /* Add the router item to the list and store its descriptor. */
    insertSorted(new RouterListItem(this, rd));
  }
}

/** Called when the selected items have changed. This emits the 
 * routerSelected() signal with the descriptor for the selected router.
 */
void
RouterListWidget::onSelectionChanged()
{
  RouterDescriptor rd;
  QList<QTreeWidgetItem *> items = selectedItems();

  if (items.count() > 0) {
      rd = ((RouterListItem *)items[0])->descriptor();
  }
    emit routerSelected(rd);
}

