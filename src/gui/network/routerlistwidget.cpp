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

#define IMG_NODE_OFFLINE    ":/images/icons/node-unresponsive.png"
#define IMG_NODE_SLEEPING   ":/images/icons/node-hibernating.png"
#define IMG_NODE_NO_BW      ":/images/icons/node-bw-none.png"
#define IMG_NODE_LOW_BW     ":/images/icons/node-bw-low.png"
#define IMG_NODE_MED_BW     ":/images/icons/node-bw-med.png"
#define IMG_NODE_HIGH_BW    ":/images/icons/node-bw-high.png"


RouterListWidget::RouterListWidget(QWidget *parent)
: QTreeWidget(parent)
{
  /* Get the TorControl object */
  _torControl = Vidalia::torControl();
 
  /* Find out when the selected item has changed. */
  connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          this, SLOT(onItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

/** Clears the list of routers. */
void
RouterListWidget::clear()
{
  _routerList.clear();
  QTreeWidget::clear();
}

/** Creates a new item for the router list, based on the given descriptor.*/
RouterListItem*
RouterListWidget::createRouterItem(RouterDescriptor rd)
{
  QIcon statusIcon;
  qint64 statusValue;
  RouterListItem *item = new RouterListItem(this);
  
  /* Decide which status icon is appropriate */
  if (rd.offline()) {
    statusValue = -1;
    statusIcon  = QIcon(IMG_NODE_OFFLINE);
  } else if (rd.hibernating()) {
    statusValue = 0;
    statusIcon  = QIcon(IMG_NODE_SLEEPING);
  } else {
    statusValue = (qint64)rd.observedBandwidth();
    if (statusValue >= 400*1024) {
      statusIcon = QIcon(IMG_NODE_HIGH_BW);
    } else if (statusValue >= 60*1024) {
      statusIcon = QIcon(IMG_NODE_MED_BW);
    } else if (statusValue >= 20*1024) {
      statusIcon = QIcon(IMG_NODE_LOW_BW);
    } else {
      statusIcon = QIcon(IMG_NODE_NO_BW);
    }
  }
  
  /* Set the icon and text */
  item->setIcon(StatusColumn, statusIcon);
  item->setData(StatusColumn, Qt::UserRole, statusValue);
  item->setText(NameColumn, rd.name());
  item->setData(NameColumn, Qt::UserRole, rd.name().toLower());
  item->setData(NameColumn, Qt::UserRole+1, rd.id());
  return item;
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

/** Finds the list item for the given descriptor. Returns 0 if not found. */
RouterListItem*
RouterListWidget::findItem(RouterDescriptor rd)
{
  QList<QTreeWidgetItem *> list = findItems(rd.name(), 
                                            Qt::MatchExactly, 
                                            NameColumn);

  /* Many routers can have the same name, so find a match on the ID. */
  foreach (QTreeWidgetItem *item, list) {
    RouterListItem *ri = (RouterListItem *)takeTopLevelItem(
                                           indexOfTopLevelItem(item));
    if (ri->data(NameColumn, Qt::UserRole+1).toString() == rd.id()) {
      return ri;
    }
  }
  return 0;
}

/** Adds a router descriptor to the list. */
void
RouterListWidget::addRouter(RouterDescriptor rd)
{
  if (!rd.name().isEmpty()) {
    if (_routerList.contains(rd.id())) {
      /* This is an updated descriptor, so remove the list item and we'll 
       * add a new, updated item. */
      QTreeWidgetItem *item = findItem(rd);
      if (item && indexOfTopLevelItem(item) != -1) {
        delete takeTopLevelItem(indexOfTopLevelItem(item));
      }
    }

    /* Add the router item to the list and store its descriptor. */
    insertSorted(createRouterItem(rd));
    _routerList.insert(rd.id(), rd);
  }
}

/** Called when the selected item is changed. This emits the routerSelected
 * signal with the descriptor for the selected router. */
void
RouterListWidget::onItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *prev)
{
  Q_UNUSED(prev);
  if (item) {
    QString id = item->data(NameColumn, Qt::UserRole+1).toString();
    emit routerSelected(_routerList.value(id));
  }
}

