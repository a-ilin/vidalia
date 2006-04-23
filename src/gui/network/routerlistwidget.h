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
 * \file routerlistwidget.h
 * \version $Id$
 */

#ifndef _ROUTERLISTWIDGET_H
#define _ROUTERLISTWIDGET_H

#include <QObject>
#include <QTreeWidget>
#include <QHash>
#include <control/torcontrol.h>

#include "routerlistitem.h"

class RouterListWidget : public QTreeWidget
{
  Q_OBJECT
  
public:
  /** Columns in the list. */
  enum Columns {
    StatusColumn = 0, /**< Status column, indicating bandwidth */
    NameColumn = 1    /*<< Router's name. */
  };

  /** Default constructor. */
  RouterListWidget(QWidget *parent = 0);
  
  /** Adds a new descriptor the list. */
  void addRouter(RouterDescriptor rd);

public slots:
  /** Clears the list of routers. */
  void clear();

signals:
  /** Called when the user selects a router from the list. */
  void routerSelected(RouterDescriptor rd);
 
private slots:
  /** Called when the user clicks on an item in the list. */
  void onItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *prev);

private:
  /** Creates a new item for the router list, based on the given descriptor.*/
  RouterListItem* createRouterItem(RouterDescriptor rd);
  /** Inserts a new item into the router list, maintaining the current order.*/
  void insertSorted(RouterListItem *item);
  /** Finds the list item for the given descriptor. */
  RouterListItem* findItem(RouterDescriptor rd);
  
  /** TorControl object used to talk to Tor. */
  TorControl* _torControl;
  /** Stores a mapping of names to router descriptors. */
  QHash<QString,RouterDescriptor> _routerList;
};

#endif

