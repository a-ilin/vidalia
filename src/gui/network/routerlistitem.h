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
 * \file routerlistitem.h
 * \version $Id$
 */

#ifndef _ROUTERLISTITEM_H
#define _ROUTERLISTITEM_H

#include <QTreeWidgetItem>
#include "routerlistwidget.h"

class RouterListWidget;

class RouterListItem : public QTreeWidgetItem
{
public:
  /** Default constructor. */
  RouterListItem(RouterListWidget *list = 0);

  /** Overload the comparison operator. */
  virtual bool operator<(const QTreeWidgetItem &other) const;

private:
  RouterListWidget* _list;  /**< The list for this list item. */
};

#endif

