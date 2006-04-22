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
 * \file netviewer.cpp
 * \version $Id: netviewer.cpp 699 2006-04-15 03:12:22Z hipplej $
 */

#include <QMessageBox>
#include <QHeaderView>
#include <vidalia.h>

#include "netviewer.h"

#define COL_STATUS    0
#define COL_NAME      1
#define IMG_NODE_OFFLINE    ":/images/icons/node-unresponsive.png"
#define IMG_NODE_SLEEPING   ":/images/icons/node-hibernating.png"
#define IMG_NODE_NO_BW      ":/images/icons/node-bw-none.png"
#define IMG_NODE_LOW_BW     ":/images/icons/node-bw-low.png"
#define IMG_NODE_MED_BW     ":/images/icons/node-bw-med.png"
#define IMG_NODE_HIGH_BW    ":/images/icons/node-bw-high.png"


/** Constructor. Loads settings from VidaliaSettings.
 * \param parent The parent widget of this NetViewer object.
 */
NetViewer::NetViewer(QWidget *parent)
: QMainWindow(parent)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Get the TorControl object */
  _torControl = Vidalia::torControl();

  /* Create the MapFrame and add it to the dialog */
  _map = new MapFrame;
  ui.gridLayout->addWidget(_map);

  /* Set the column widths on the router list */
  ui.treeRouterList->header()->resizeSection(COL_STATUS, 55);
  ui.treeRouterList->sortItems(COL_NAME, Qt::AscendingOrder);
  
  /* Connect the necessary slots and signals */
  connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui.actionNewNym, SIGNAL(triggered()), this, SLOT(newNym()));
  connect(_torControl, SIGNAL(connected(bool)), 
          ui.actionNewNym, SLOT(setEnabled(bool)));
  connect(ui.actionRefresh, SIGNAL(triggered()), this, SLOT(loadRouterList()));
  connect(_torControl, SIGNAL(connected(bool)),
          ui.actionRefresh, SLOT(setEnabled(bool)));
}

/** Overloads the default show() slot. */
void
NetViewer::show()
{
  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    QMainWindow::activateWindow();
    QMainWindow::raise();
  }
}

/** Called when the user selects the "Help" action from the toolbar. */
void
NetViewer::help()
{
  Vidalia::help("netview");
}

/** Called when the user selects the "New Nym" action from the toolbar. */
void
NetViewer::newNym()
{
  QString errmsg;
  if (_torControl->signal(TorSignal::NewNym, &errmsg)) {
    QMessageBox::information(this, 
      tr("New Nym"), 
      tr("All subsequent application requests will use new circuits."),
      QMessageBox::Ok, QMessageBox::NoButton);
  } else {
    QMessageBox::warning(this,
      tr("New Nym Failed"), errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/** Creates a new item for the router list, based on the given descriptor.*/
QTreeWidgetItem*
NetViewer::createRouterItem(RouterDescriptor rd)
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
  item->setText(COL_NAME, rd.name());
  return item;
}

/** Loads a list of router's that Tor knows about. */
void
NetViewer::loadRouterList()
{
  QList<RouterDescriptor> routerList = _torControl->getRouterList();
  
  /* Clear the existing list of routers and descriptors */
  ui.treeRouterList->clear();
  _routerList.clear();

  /* Create an item for each router and associate it with a descriptor */
  foreach (RouterDescriptor rd, routerList) {
    ui.treeRouterList->addTopLevelItem(createRouterItem(rd));
    _routerList.insert(rd.name(), rd);
  }
  ui.treeRouterList->sortByColumn(ui.treeRouterList->sortColumn());
}

