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
 * \file routerlistitem.cpp
 * \version $Id$
 * \brief Item representing a single router and status in a RouterListWidget
 */

#include "routerlistwidget.h"
#include "routerlistitem.h"

#define STATUS_COLUMN (RouterListWidget::StatusColumn)
#define NAME_COLUMN   (RouterListWidget::NameColumn)

#define IMG_NODE_OFFLINE    ":/images/icons/node-unresponsive.png"
#define IMG_NODE_SLEEPING   ":/images/icons/node-hibernating.png"
#define IMG_NODE_NO_BW      ":/images/icons/node-bw-none.png"
#define IMG_NODE_LOW_BW     ":/images/icons/node-bw-low.png"
#define IMG_NODE_MED_BW     ":/images/icons/node-bw-med.png"
#define IMG_NODE_HIGH_BW    ":/images/icons/node-bw-high.png"


/** Default constructor. */
RouterListItem::RouterListItem(RouterListWidget *list, RouterDescriptor rd)
: QTreeWidgetItem()
{
  _list = list;
  _rd   = 0;
  update(rd);
}

/** Destructor. */
RouterListItem::~RouterListItem()
{
  delete _rd;
}

/** Updates the router descriptor for this item. */
void
RouterListItem::update(RouterDescriptor rd)
{
  QIcon statusIcon;
  if (_rd) {
    delete _rd;
  }
  _rd = new RouterDescriptor(rd);
  
  /* Determine the status value (used for sorting) and icon */
  if (_rd->offline()) {
    _statusValue = -1;
    statusIcon = QIcon(IMG_NODE_OFFLINE);
    setToolTip(STATUS_COLUMN, tr("Offline"));
  } else if (_rd->hibernating()) {
    _statusValue = 0;
    statusIcon = QIcon(IMG_NODE_SLEEPING);
    setToolTip(STATUS_COLUMN, tr("Hibernating"));
  } else {
    _statusValue = (qint64)_rd->observedBandwidth();
    if (_statusValue >= 400*1024) {
      statusIcon = QIcon(IMG_NODE_HIGH_BW);
    } else if (_statusValue >= 60*1024) {
      statusIcon = QIcon(IMG_NODE_MED_BW);
    } else if (_statusValue >= 20*1024) {
      statusIcon = QIcon(IMG_NODE_LOW_BW);
    } else {
      statusIcon = QIcon(IMG_NODE_NO_BW);
    }
    setToolTip(STATUS_COLUMN, tr("%1 KB/s").arg(_statusValue/1024));
  }
  
  /* Make the new information visible */
  setIcon(STATUS_COLUMN, statusIcon);
  setText(NAME_COLUMN, _rd->name());
  setToolTip(NAME_COLUMN, QString(_rd->name() + "\r\n" + _rd->platform()));
}

/** Sets the location information for this item's router descriptor. */
void
RouterListItem::setLocation(QString location)
{
  _rd->setLocation(location);
}

/** Overload the comparison operator. */
bool
RouterListItem::operator<(const QTreeWidgetItem &other) const
{
  const RouterListItem *a = this;
  const RouterListItem *b = (RouterListItem *)&other;

  if (_list) {
    if (_list->sortColumn() == RouterListWidget::StatusColumn) {
      /* Numeric comparison based on status and/or bandwidth */
      return (a->_statusValue < b->_statusValue);
    } else if (_list->sortColumn() == RouterListWidget::NameColumn) {
      /* Perform a case-insensitive comparison based on router name */
      return (a->name().toLower() < b->name().toLower());
    }
  }
  return QTreeWidgetItem::operator<(other);
}

