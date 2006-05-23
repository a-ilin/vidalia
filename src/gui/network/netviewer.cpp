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

#define FONT        QFont(tr("Arial"), 10)
#define IMG_MAP     ":/images/map/world-map.png"
#define IMG_MOVE    ":/images/22x22/move-map.png"
#define IMG_ZOOMIN  ":/images/22x22/zoom-in.png"
#define IMG_ZOOMOUT ":/images/22x22/zoom-out.png"

/** Maximum time to proceess other events while loading the long list of
 * router descriptors. */
#define MAX_EVENTS_TIMEOUT  25


/** Constructor. Loads settings from VidaliaSettings.
 * \param parent The parent widget of this NetViewer object.\
 */
NetViewer::NetViewer(QWidget *parent)
: QMainWindow(parent)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Get the TorControl object */
  _torControl = Vidalia::torControl();
  _torControl->setEvent(TorEvents::NewDescriptor, this, true);
  _torControl->setEvent(TorEvents::CircuitStatus, this, true);
  _torControl->setEvent(TorEvents::StreamStatus,  this, true);
  
  /* Create the ZImageView  and add it to the dialog */
  _map = new ZImageView();
  QImage image(IMG_MAP);
  _map->setImage(image);
  ui.gridLayout->addWidget(_map);
 
  /* Add the map manipulation action group to the toolbar */
  QActionGroup *grp = new QActionGroup(this);
  createAction(QIcon(IMG_ZOOMIN), tr("Zoom In"), grp);
  createAction(QIcon(IMG_ZOOMOUT), tr("Zoom Out"), grp);
  createAction(QIcon(IMG_MOVE), tr("Move"), grp);
  foreach (QAction *act, grp->actions()) {
    ui.toolBar->insertAction(ui.actionHelp, act);
  }
  ui.toolBar->insertSeparator(ui.actionHelp);
  connect(grp, SIGNAL(triggered(QAction *)),
          this, SLOT(NULL));
  
  /* Create the timer that will be used to update the router list once every
   * hour. We still receive the NEWDESC event to get new descriptors, but this
   * needs to be called to get rid of any descriptors that were removed. */
  _timer = new QTimer(this);
  _timer->setInterval(60*60*1000);
  connect(_timer, SIGNAL(timeout()), this, SLOT(loadRouters()));
  connect(_torControl, SIGNAL(connected()), _timer, SLOT(start()));
  connect(_torControl, SIGNAL(disconnected()), _timer, SLOT(stop()));

  /* Connect the necessary slots and signals */
  connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui.actionNewNym, SIGNAL(triggered()), this, SLOT(newNym()));
  connect(ui.actionRefresh, SIGNAL(triggered()), this, SLOT(loadRouters()));
  connect(ui.treeRouterList, SIGNAL(routerSelected(QList<RouterDescriptor>)),
          ui.textRouterInfo, SLOT(display(QList<RouterDescriptor>)));
  connect(ui.treeCircuitList, SIGNAL(circuitSelected(Circuit)),
          this, SLOT(circuitSelected(Circuit)));

  connect(_torControl, SIGNAL(connected(bool)), ui.actionRefresh, SLOT(setEnabled(bool)));
  connect(_torControl, SIGNAL(connected(bool)), ui.actionNewNym, SLOT(setEnabled(bool)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(loadRouters()));
  connect(_torControl, SIGNAL(connected()), this, SLOT(loadConnections())); 
  connect(_torControl, SIGNAL(disconnected()), ui.treeCircuitList, SLOT(clear()));
}

/** Creates and adds an action to the specified action group */
void
NetViewer::createAction(QIcon img, QString text, QActionGroup *group)
{
  QAction *action = new QAction(img, text, group);
  action->setCheckable(true);
  action->setFont(FONT);
}

/** Custom event handler. Catches the new descriptor events. */
void
NetViewer::customEvent(QEvent *event)
{
  int type = event->type();
  if (type == CustomEventType::NewDescriptorEvent) {
    /* New router descriptor, so load it and add it to the list */
    NewDescriptorEvent *nde = (NewDescriptorEvent *)event;
    loadNewDescriptors(nde->descriptorIDs());
  } else if (type == CustomEventType::CircuitEvent) {
    /* New or updated circuit information */
    CircuitEvent *ce = (CircuitEvent *)event;
    ui.treeCircuitList->addCircuit(ce->circuit());
  } else if (type == CustomEventType::StreamEvent) {
    /* New or updated stream information */
    StreamEvent *se = (StreamEvent *)event;
    ui.treeCircuitList->addStream(se->stream());
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
    Vidalia::processEvents(QEventLoop::AllEvents, MAX_EVENTS_TIMEOUT);
  }

  /* Ok, they can refresh again. */
  ui.actionRefresh->setEnabled(true);
}

/** Loads a list of all current circuits and streams. */
void
NetViewer::loadConnections()
{
  /* Load all circuits */
  QList<Circuit> circuits = _torControl->getCircuits();
  foreach (Circuit circuit, circuits) {
    ui.treeCircuitList->addCircuit(circuit);
  }
  /* Now load all streams */
  QList<Stream> streams = _torControl->getStreams();
  foreach (Stream stream, streams) {
    ui.treeCircuitList->addStream(stream);
  }
}

/** Overloads the default show() slot. */
void
NetViewer::show()
{
  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    activateWindow();
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    raise();
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
    Vidalia::processEvents(QEventLoop::AllEvents, MAX_EVENTS_TIMEOUT);
  }
}

/** Called when the user selects a circuit from the circuit and streams
 * list. */
void
NetViewer::circuitSelected(Circuit circuit)
{
  /* Clear any selected items. */
  ui.treeRouterList->deselectAll();

  foreach (QString router, circuit.hops()) {
    /* Try to find and select each router in the path */
    RouterListItem *item = ui.treeRouterList->findRouterItem(router);
    if (item) {
      ui.treeRouterList->setItemSelected(item, true);
    }
  }
}

