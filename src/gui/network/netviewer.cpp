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
  _torControl->setEvent(TorEvents::NewDescriptor, this, true);

  /* Create the MapFrame and add it to the dialog */
  _map = new MapFrame;
  ui.gridLayout->addWidget(_map);

  /* Set the column size and sort order for the router list. */
  ui.treeRouterList->header()->resizeSection(
                                RouterListWidget::StatusColumn, 55);
  ui.treeRouterList->sortByColumn(RouterListWidget::NameColumn);

  /* Connect the necessary slots and signals */
  connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui.actionNewNym, SIGNAL(triggered()), this, SLOT(newNym()));
  connect(_torControl, SIGNAL(connected(bool)), 
          ui.actionNewNym, SLOT(setEnabled(bool)));
  connect(ui.actionRefresh, SIGNAL(triggered()), 
          this, SLOT(loadRouters()));
  connect(_torControl, SIGNAL(connected(bool)),
          ui.actionRefresh, SLOT(setEnabled(bool)));
  connect(ui.treeRouterList, SIGNAL(routerSelected(RouterDescriptor)),
          ui.textRouterInfo, SLOT(display(RouterDescriptor)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(loadRouters()));
}

/** Custom event handler. Catches the new descriptor events. */
void
NetViewer::customEvent(QEvent *event)
{
  if (event->type() == CustomEventType::NewDescriptorEvent) {
    NewDescriptorEvent *nde = (NewDescriptorEvent *)event;
    loadNewDescriptors(nde->descriptorIDs());
  }
}

/** Loads a list of router's that Tor knows about. */
void
NetViewer::loadRouters()
{
  QStringList idList;

  /* Don't let the user refresh while we're refreshing. */
  ui.actionRefresh->setEnabled(false);
  
  /* Clear the existing list of routers and descriptors */
  ui.treeRouterList->clear();
 
  /* Create an item for each router and associate it with a descriptor */
  idList = _torControl->getRouterIDList();
  foreach (QString id, idList) {
    ui.treeRouterList->addRouter(_torControl->getRouterDescriptor(id));
    Vidalia::processEvents();
  }

  /* Ok, they can refresh again. */
  ui.actionRefresh->setEnabled(true);
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

/** Loads a list of new descriptors from the given IDs. */
void
NetViewer::loadNewDescriptors(QStringList ids)
{
  foreach (QString id, ids) {
    ui.treeRouterList->addRouter(_torControl->getRouterDescriptor(id));
    Vidalia::processEvents();
  }
}

