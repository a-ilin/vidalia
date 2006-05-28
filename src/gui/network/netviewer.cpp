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
  
  /* Change the column widths of the tree widgets */
  ui.treeRouterList->header()->
    resizeSection(RouterListWidget::StatusColumn,
		  RouterListWidget::StatusColumnWidth);
  ui.treeCircuitList->header()->
    resizeSection(CircuitListWidget::ConnectionColumn,
		  CircuitListWidget::ConnectionColumnWidth);

  /* Create the TorMapWidget and add it to the dialog */
  _map = new TorMapWidget();
  ui.gridLayout->addWidget(_map);

  /* Connect zoom buttons to ZImageView zoom slots */
  connect(ui.actionZoomIn, SIGNAL(triggered()), _map, SLOT(zoomIn()));
  connect(ui.actionZoomOut, SIGNAL(triggered()), _map, SLOT(zoomOut()));
  
  /* Create the timer that will be used to update the router list once every
   * hour. We still receive the NEWDESC event to get new descriptors, but this
   * needs to be called to get rid of any descriptors that were removed. */
  _timer = new QTimer(this);
  _timer->setInterval(60*60*1000);
  connect(_timer, SIGNAL(timeout()), this, SLOT(loadRouterList()));
  connect(_torControl, SIGNAL(connected()), _timer, SLOT(start()));
  connect(_torControl, SIGNAL(disconnected()), _timer, SLOT(stop()));

  /* Connect the necessary slots and signals */
  connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui.actionNewNym, SIGNAL(triggered()), this, SLOT(newNym()));
  connect(ui.actionRefresh, SIGNAL(triggered()), this, SLOT(loadRouterList()));
  connect(ui.treeRouterList, SIGNAL(routerSelected(QList<RouterDescriptor>)),
          ui.textRouterInfo, SLOT(display(QList<RouterDescriptor>)));
  connect(ui.treeCircuitList, SIGNAL(circuitSelected(Circuit)),
          this, SLOT(circuitSelected(Circuit)));
  
  /* Respond to user actions on the map. */
  connect(_map, SIGNAL(circuitSelected(Circuit)), 
          this,   SLOT(circuitSelected(Circuit)));
  connect(_map, SIGNAL(routerSelected(QString)),
          this,   SLOT(mapRouterSelected(QString)));

  /* Respond to changes in the status of the control connection */
  connect(_torControl, SIGNAL(connected(bool)), ui.actionRefresh, SLOT(setEnabled(bool)));
  connect(_torControl, SIGNAL(connected(bool)), ui.actionNewNym, SLOT(setEnabled(bool)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(loadRouterList()));
  connect(_torControl, SIGNAL(connected()), this, SLOT(loadConnections())); 
  connect(_torControl, SIGNAL(disconnected()), ui.treeCircuitList, SLOT(clear()));

  /* Connect the slot to find out when geoip information has arrived */
  connect(&_geoip, SIGNAL(resolved(int, QList<GeoIp>)), 
             this,   SLOT(resolved(int, QList<GeoIp>)));
}

/** Custom event handler. Catches the new descriptor events. */
void
NetViewer::customEvent(QEvent *event)
{
  int type = event->type();
  if (type == CustomEventType::NewDescriptorEvent) {
    /* New router descriptor, so load it and add it to the list */
    NewDescriptorEvent *nde = (NewDescriptorEvent *)event;
    loadDescriptors(nde->descriptorIDs());
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
NetViewer::loadRouterList()
{
  QStringList idList;

  /* Don't let the user refresh while we're refreshing. */
  ui.actionRefresh->setEnabled(false);
  
  /* Clear the existing list of routers and descriptors */
  ui.treeRouterList->clear();
 
  /* Create an item for each router and associate it with a descriptor */
  loadDescriptors(_torControl->getRouterIDList()); 

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
      tr("New Identity"), 
      tr("All subsequent connections will appear to be different "
         "than your old connections."),
      QMessageBox::Ok, QMessageBox::NoButton);
  } else {
    QMessageBox::warning(this,
      tr("Failed to Create New Identity"), errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/** Loads a list of new descriptors from the given IDs. */
void
NetViewer::loadDescriptors(QStringList ids)
{
  QList<QHostAddress> iplist;
  
  foreach (QString id, ids) {
    /* Load the router descriptor and add it to the router list. */
    RouterDescriptor rd = _torControl->getRouterDescriptor(id);
    if (!rd.isEmpty()) {
      ui.treeRouterList->addRouter(rd);

      /* Add this IP to a list whose geographic location we'd like to find. */
      QHostAddress ip(rd.ip());
      if (!iplist.contains(ip)) {
        iplist << ip;
      }
    }

    /* Process pending events for a bit so the GUI remains responsive */
    Vidalia::processEvents(QEventLoop::AllEvents, MAX_EVENTS_TIMEOUT);
    /* If the connection broke, then bail */
    if (!_torControl->isConnected()) {
      return;
    }
  }

  /* If there are any IPs in the list, then resolve them to locations */
  if (iplist.size() > 0) {
    _geoip.resolve(iplist);
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

/** Called when the user selects a router on the map. */
void
NetViewer::mapRouterSelected(QString name)
{
  RouterListItem *item = ui.treeRouterList->findRouterItem(name);
  if (item) {
    ui.treeRouterList->deselectAll();
    ui.treeRouterList->setItemSelected(item, true);
  }
}

/** Called when a list of GeoIp information has been resolved. */
void
NetViewer::resolved(int id, QList<GeoIp> geoips)
{
  Q_UNUSED(id);
  QList<RouterListItem *> routers;
  
  foreach (GeoIp geoip, geoips) {
    /* Do something with our awesome new information */
    routers = ui.treeRouterList->findRouterItems(geoip.ip());
    foreach (RouterListItem *item, routers) {
      item->setLocation(geoip.toLocation());
    }
  }
}

