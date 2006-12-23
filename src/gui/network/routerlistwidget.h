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
 * \brief Displays a list of Tor servers and their status
 */

#ifndef _ROUTERLISTWIDGET_H
#define _ROUTERLISTWIDGET_H

#include <QHash>
#include <QMenu>
#include <QObject>
#include <QAction>
#include <QKeyEvent>
#include <QTreeWidget>
#include <QHostAddress>
#include <QMouseEvent>

#include "routerlistitem.h"


class RouterListWidget : public QTreeWidget
{
  Q_OBJECT
  
public:
  /** Columns in the list. */
  enum Columns {
    StatusColumn  = 0,  /**< Status column, indicating bandwidth. */
    CountryColumn = 1,  /**< Router's country flag. */
    NameColumn    = 2,  /**< Router's name. */
    
  };

  /** Default constructor. */
  RouterListWidget(QWidget *parent = 0);
  
  /** Adds a new descriptor the list. */
  void addRouter(RouterDescriptor rd);
  /** Finds the list item whose router nickname matches <b>name</b>. If more
   * than one router exists with given name, the first match will be
   * returned.  Returns 0 if not found. */
  RouterListItem* findRouterByName(QString name);
  /** Finds the list item whose key ID matches <b>id</b>. Returns 0 if not 
   * found. */
  RouterListItem* findRouterById(QString id);
  /** Deselects all currently selected routers. */
  void deselectAll();

signals:
  /** Emitted when the user selects a router from the list. */
  void routerSelected(RouterDescriptor rd);
  /** Emitted when the user selects a router to zoom in on. */
  void zoomToRouter(QString id);
  
public slots:
  /** Clears the list of router items. */
  void clearRouters();
 
protected:
  /** Called when the user presses and releases a moust button. */
  virtual void mouseReleaseEvent(QMouseEvent *e);
  
private slots:
  /** Called when the user clicks on an item in the list. */
  void onSelectionChanged();

protected:
  /** Called when the user presses a key while the list has focus. */
  void keyPressEvent(QKeyEvent *event);

private:
  /** Inserts a new item into the router list, maintaining the current order.*/
  void insertSorted(RouterListItem *item);

  /** Maps a server ID to that server's list item. */
  QHash<QString,RouterListItem*> _idmap;
  
  /** Router item context menu and items. */
  QMenu* _routerContextMenu; /**< Context menu for router items. */
  QAction* _zoomToRouterAct; /**< Zooms in on the selected router. */
};

#endif

