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
** \file routerlistwidget.cpp
** \version $Id$
** \brief Displays a list of Tor servers and their status
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
                                << tr("Relay"));

  /* Sort by descending server bandwidth */
  sortItems(StatusColumn, Qt::DescendingOrder);

  /* Find out when the selected item has changed. */
  connect(this, SIGNAL(itemSelectionChanged()), 
          this, SLOT(onSelectionChanged()));
  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(customContextMenuRequested(QPoint)));
}

/** Called when the user requests a context menu for a router in the list. A
 * context menu will be displayed providing a list of actions, including
 * zooming in on the server. */
void
RouterListWidget::customContextMenuRequested(const QPoint &pos)
{
  QMenu menu(this);

  /* Find out which (if any) router in the list is selected */
  RouterListItem *item = dynamic_cast<RouterListItem *>(itemAt(pos));
  if (!item)
    return;
  
  /* Set up the context menu */
  QAction *zoomAction =
    new QAction(QIcon(IMG_ZOOM), tr("Zoom to Relay"), &menu);
  menu.addAction(zoomAction);
  
  /* Display the menu and find out which (if any) action was selected */
  QAction *action = menu.exec(mapToGlobal(pos));
  if (action == zoomAction)
    emit zoomToRouter(item->id());
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
  setStatusTip(tr("%1 relays online").arg(0));
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
  QList<QTreeWidgetItem *> list = findItems(name, Qt::MatchExactly,
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
  if (id.isEmpty())
    return;

  RouterListItem *item = findRouterById(id);
  if (item) {
    item->update(rd);
  } else {
    item = new RouterListItem(this, rd);
    addTopLevelItem(item);
    _idmap.insert(id, item);
  }

  /* Set our status tip to the number of servers in the list */
  setStatusTip(tr("%1 relays online").arg(topLevelItemCount()));
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

