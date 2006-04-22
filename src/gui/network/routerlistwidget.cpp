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

#define COL_STATUS    0
#define COL_NAME      1
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
 
  /* Set the column widths on the router list */
  header()->resizeSection(COL_STATUS, 55);
  sortItems(COL_NAME, Qt::AscendingOrder);

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
QTreeWidgetItem*
RouterListWidget::createRouterItem(RouterDescriptor rd)
{
  QIcon statusIcon;
  QTreeWidgetItem *item = new QTreeWidgetItem();
  
  /* Decide which status icon is appropriate */
  if (rd.offline()) {
    statusIcon = QIcon(IMG_NODE_OFFLINE);
  } else if (rd.hibernating()) {
    statusIcon = QIcon(IMG_NODE_SLEEPING);
  } else if (rd.averageBandwidth() >= 400*1024) {
    statusIcon = QIcon(IMG_NODE_HIGH_BW);
  } else if (rd.averageBandwidth() >= 60*1024) {
    statusIcon = QIcon(IMG_NODE_MED_BW);
  } else if (rd.averageBandwidth() >= 20*1024) {
    statusIcon = QIcon(IMG_NODE_LOW_BW);
  } else {
    statusIcon = QIcon(IMG_NODE_NO_BW);
  }
  
  /* Set the icon and text */
  item->setIcon(COL_STATUS, statusIcon);
  item->setText(COL_STATUS, "");
  item->setText(COL_NAME, rd.name());
  return item;
}

/** Adds a router descriptor to the list. */
void
RouterListWidget::addRouter(RouterDescriptor rd)
{
  addTopLevelItem(createRouterItem(rd));
  _routerList.insert(rd.name(), rd);
  sortByColumn(sortColumn());
}

/** Called when the selected item is changed. This emits the routerSelected
 * signal with the descriptor for the selected router. */
void
RouterListWidget::onItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *prev)
{
  Q_UNUSED(prev);
  if (item) {
    emit routerSelected(_routerList.value(item->text(COL_NAME)));
  }
}

