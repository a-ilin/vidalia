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
 * \brief Displays a list of Tor servers and their status
 */

#include <QHeaderView>
#include <vidalia.h>

#include "routerlistwidget.h"

#define IMG_ZOOM   ":/images/16x16/zoom.png"


RouterListWidget::RouterListWidget(QWidget *parent)
: QTreeWidget(parent)
{
  /* Create and initialize columns */
  setHeaderLabels(QStringList() << QString("")
                                << QString("")
                                << tr("Server"));

  /* Sort by descending server bandwidth */
  sortItems(StatusColumn, Qt::DescendingOrder);

  /* Find out when the selected item has changed. */
  connect(this, SIGNAL(itemSelectionChanged()), 
          this, SLOT(onSelectionChanged()));

  /* Set up the router item context menu */
  _routerContextMenu = new QMenu(this);
  _zoomToRouterAct = new QAction(QIcon(IMG_ZOOM), tr("Zoom to Server"), this);
  _routerContextMenu->addAction(_zoomToRouterAct);
}

/** Called when the user presses and releases a mouse button. If the event
 * indicates a right-click on a router item, a context menu will be displayed
 * providing a list of actions, including zooming in on the server. */
void
RouterListWidget::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::RightButton) {
    /* Find out which server was right-clicked */
    RouterListItem *item = (RouterListItem *)itemAt(e->pos());
    if (!item) {
      return;
    }

    /* Display a context menu for this router item */
    QPoint pos = e->globalPos();
    QAction *action = _routerContextMenu->exec(pos);
    if (action == _zoomToRouterAct) {
      /* Zoom in on this router */
      emit zoomToRouter(item->id());
    }
  }
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

/** Clear the list of router items. */
void
RouterListWidget::clearRouters()
{
  _idmap.clear();
  QTreeWidget::clear();
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
  int index;
  
  QString key = event->text();
  if (!key.isEmpty() && key.at(0).isLetterOrNumber()) {
    /* A text key was pressed, so search for routers that begin with that key. */
    QList<QTreeWidgetItem *> list = findItems(QString("^[%1%2].*$")
                                                  .arg(key.toUpper())
                                                  .arg(key.toLower()),
                                               Qt::MatchRegExp|Qt::MatchWrap,
                                               NameColumn);
    if (list.size() > 0) {
      QList<QTreeWidgetItem *> s = selectedItems();
      
      /* A match was found, so deselect any previously selected routers,
       * select the new match, and make sure it's visible. If there was
       * already a router selected that started with the search key, go to the
       * next match in the list. */
      deselectAll();
      index = (!s.size() ? 0 : (list.indexOf(s.at(0)) + 1) % list.size());

      /* Select the item and scroll to it */
      setItemSelected(list.at(index), true);
      scrollToItem(list.at(index));
    }
    event->accept();
  } else {
    /* It was something we don't understand, so hand it to the parent class */
    QTreeWidget::keyPressEvent(event);
  }
}

/** Finds the list item whose key ID matches <b>id</b>. Returns 0 if not 
 * found. */
RouterListItem*
RouterListWidget::findRouterById(QString id)
{
  if (_idmap.contains(id)) {
    return _idmap.value(id);
  }
  return 0;
}

/** Finds the list item whose router nickname matches <b>name</b>. If more 
 * than one router exists with given name, the first match will be returned. 
 * Returns 0 if not found. */
RouterListItem*
RouterListWidget::findRouterByName(QString name)
{
  QList<QTreeWidgetItem *> list = findItems(name,
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

/** Adds a router descriptor to the list. */
void
RouterListWidget::addRouter(RouterDescriptor rd)
{
  QString id = rd.id();
  if (!id.isEmpty()) {
    RouterListItem *item = findRouterById(id);
    if (item) {
      /* This is an updated descriptor, so remove the old item and we'll 
       * add a new, updated item. */
      delete takeTopLevelItem(indexOfTopLevelItem(item));
      _idmap.remove(id);
    }

    /* Add the router item to the list and store its descriptor. */
    item = new RouterListItem(this, rd);
    insertSorted(item);
    _idmap.insert(id, item);

    /* Set our status tip to the number of servers in the list */
    setStatusTip(tr("%1 servers total").arg(topLevelItemCount()));
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

