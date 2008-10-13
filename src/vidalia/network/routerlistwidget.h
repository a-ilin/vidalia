/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file routerlistwidget.h
** \version $Id$
** \brief Displays a list of Tor servers and their status
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
  /** Finds the list item whose key ID matches <b>id</b>. Returns 0 if not 
   * found. */
  RouterListItem* findRouterById(QString id);
  /** Deselects all currently selected routers. */
  void deselectAll();
  /** Called when the user changes the UI translation. */
  void retranslateUi();

signals:
  /** Emitted when the user selects a router from the list. */
  void routerSelected(RouterDescriptor rd);
  /** Emitted when the user selects a router to zoom in on. */
  void zoomToRouter(QString id);
  
public slots:
  /** Clears the list of router items. */
  void clearRouters();
 
private slots:
  /** Called when the user clicks on an item in the list. */
  void onSelectionChanged();
  /** Called when the user requests a context menu for some router in the
   * list. */  
  void customContextMenuRequested(const QPoint &pos);

protected:
  /** Called when the user presses a key while the list has focus. */
  void keyPressEvent(QKeyEvent *event);

private:
  /** Maps a server ID to that server's list item. */
  QHash<QString,RouterListItem*> _idmap;
};

#endif

