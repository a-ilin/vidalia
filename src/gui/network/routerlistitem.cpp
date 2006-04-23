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
 */

#include "routerlistwidget.h"
#include "routerlistitem.h"

/** Default constructor. */
RouterListItem::RouterListItem(RouterListWidget *list)
: QTreeWidgetItem()
{
  _list = list;
}

/** Overload the comparison operator. */
bool
RouterListItem::operator<(const QTreeWidgetItem &other) const
{
  if (_list) {
    if (_list->sortColumn() == RouterListWidget::StatusColumn) {
      /* Numeric comparison based on status and/or bandwidth */
      return (this->data(RouterListWidget::StatusColumn,
                         Qt::UserRole).toLongLong() >
              other.data(RouterListWidget::StatusColumn,
                         Qt::UserRole).toLongLong());
    } else if (_list->sortColumn() == RouterListWidget::NameColumn) {
      /* Perform a case-insensitive comparison based on router name */
      return (this->data(RouterListWidget::NameColumn,
                         Qt::UserRole).toString() >
              other.data(RouterListWidget::NameColumn,
                         Qt::UserRole).toString());
    }
  }
  return QTreeWidgetItem::operator<(other);
}

