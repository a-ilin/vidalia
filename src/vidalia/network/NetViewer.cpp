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
** \file NetViewer.cpp
** \version $Id$
** \brief Displays a map of the Tor network and the user's circuits
*/

#include "NetViewer.h"
#include "RouterInfoDialog.h"
#include "RouterListItem.h"
#include "Vidalia.h"
#include "VMessageBox.h"

#include <QMessageBox>
#include <QHeaderView>

#define IMG_MOVE    ":/images/22x22/move-map.png"
#define IMG_ZOOMIN  ":/images/22x22/zoom-in.png"
#define IMG_ZOOMOUT ":/images/22x22/zoom-out.png"

/** Number of milliseconds to wait after the arrival of the last descriptor whose
 * IP needs to be resolved to geographic information, in case more descriptors
 * arrive. Then we can simply lump the IPs into a single request. */
#define MIN_RESOLVE_QUEUE_DELAY   (10*1000)
/** Maximum number of milliseconds to wait after the arrival of the first
 * IP address into the resolve queue, before we flush the entire queue. */
#define MAX_RESOLVE_QUEUE_DELAY   (30*1000)


/** Constructor. Loads settings from VidaliaSettings.
 * \param parent The parent widget of this NetViewer object.\
 */
NetViewer::NetViewer(QWidget *parent)
  : VidaliaWindow("NetViewer", parent)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

#if defined(Q_WS_MAC)
  ui.actionHelp->setShortcut(QString("Ctrl+?"));
#endif

  /* Pressing 'Esc' or 'Ctrl+W' will close the window */
  ui.actionClose->setShortcut(QString("Esc"));
  Vidalia::createShortcut("Ctrl+W", this, ui.actionClose, SLOT(trigger()));

  /* Get the TorControl object */
  _torControl = Vidalia::torControl();
  _torControl->setEvent(TorEvents::NewDescriptor, this, true);
  _torControl->setEvent(TorEvents::CircuitStatus, this, true);
  _torControl->setEvent(TorEvents::StreamStatus,  this, true);
  _torControl->setEvent(TorEvents::AddressMap,    this, true);

  /* Change the column widths of the tree widgets */
  ui.treeRouterList->header()->
    resizeSection(RouterListWidget::StatusColumn, 25);
  ui.treeRouterList->header()->
    resizeSection(RouterListWidget::CountryColumn, 25);
  ui.treeCircuitList->header()->
    resizeSection(CircuitListWidget::ConnectionColumn, 235);

  /* Create the TorMapWidget and add it to the dialog */
#if defined(USE_MARBLE)
  _map = new TorMapWidget();
  connect(_map, SIGNAL(displayRouterInfo(QString)),
          this, SLOT(displayRouterInfo(QString)));
  connect(ui.actionZoomFullScreen, SIGNAL(triggered()),
          this, SLOT(toggleFullScreen()));
  Vidalia::createShortcut("ESC", _map, this, SLOT(toggleFullScreen()));
#else
  _map = new TorMapImageView();
  ui.actionZoomFullScreen->setVisible(false);
#endif
  ui.gridLayout->addWidget(_map);


  /* Connect zoom buttons to TorMapWidget zoom slots */
  connect(ui.actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
  connect(ui.actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
  connect(ui.actionZoomToFit, SIGNAL(triggered()), _map, SLOT(zoomToFit()));

  /* Create the timer that will be used to update the router list once every
   * hour. We still receive the NEWDESC event to get new descriptors, but this
   * needs to be called to get rid of any descriptors that were removed. */
  _refreshTimer.setInterval(60*60*1000);
  connect(&_refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
  
  /* Set up the timers used to group together GeoIP requests for new
   * descriptors arriving within MIN_RESOLVE_QUEUE_DELAY milliseconds, but no
   * more than MAX_RESOLVE_QUEUE_DELAY milliseconds of each other. */
  _minResolveQueueTimer.setSingleShot(true);
  connect(&_minResolveQueueTimer, SIGNAL(timeout()), this, SLOT(resolve()));
  _maxResolveQueueTimer.setSingleShot(true);
  connect(&_maxResolveQueueTimer, SIGNAL(timeout()), this, SLOT(resolve()));

  /* Connect the necessary slots and signals */
  connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui.actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
  connect(ui.treeRouterList, SIGNAL(routerSelected(QList<RouterDescriptor>)),
	        this, SLOT(routerSelected(QList<RouterDescriptor>)));
  connect(ui.treeRouterList, SIGNAL(zoomToRouter(QString)),
          _map, SLOT(zoomToRouter(QString)));
  connect(ui.treeCircuitList, SIGNAL(circuitSelected(Circuit)),
          this, SLOT(circuitSelected(Circuit)));
  connect(ui.treeCircuitList, SIGNAL(circuitRemoved(CircuitId)),
          _map, SLOT(removeCircuit(CircuitId)));
  connect(ui.treeCircuitList, SIGNAL(zoomToCircuit(CircuitId)),
          _map, SLOT(zoomToCircuit(CircuitId)));
  connect(ui.treeCircuitList, SIGNAL(closeCircuit(CircuitId)),
          _torControl, SLOT(closeCircuit(CircuitId)));
  connect(ui.treeCircuitList, SIGNAL(closeStream(StreamId)),
          _torControl, SLOT(closeStream(StreamId)));

  /* Respond to changes in the status of the control connection */
  connect(_torControl, SIGNAL(authenticated()), this, SLOT(onAuthenticated()));
  connect(_torControl, SIGNAL(disconnected()), this, SLOT(onDisconnected())); 

  /* Connect the slot to find out when geoip information has arrived */
  connect(&_geoip, SIGNAL(resolved(int, QList<GeoIp>)), 
             this,   SLOT(resolved(int, QList<GeoIp>)));
}

/** Called when the user changes the UI translation. */
void
NetViewer::retranslateUi()
{
  ui.retranslateUi(this);
  ui.treeRouterList->retranslateUi();
  ui.treeCircuitList->retranslateUi();

  if (ui.treeRouterList->selectedItems().size()) {
    QList<RouterDescriptor> routers;
    foreach (QTreeWidgetItem *item, ui.treeRouterList->selectedItems()) {
      routers << dynamic_cast<RouterListItem *>(item)->descriptor();
    }
    ui.textRouterInfo->display(routers);
  } else if (ui.treeCircuitList->selectedItems().size()) {
    QList<RouterDescriptor> routers;
    QTreeWidgetItem *item = ui.treeCircuitList->selectedItems()[0];
    Circuit circuit = dynamic_cast<CircuitItem*>(item)->circuit();
    foreach (QString id, circuit.routerIDs()) {
      RouterListItem *item = ui.treeRouterList->findRouterById(id);
      if (item)
        routers.append(item->descriptor());
    }
    ui.textRouterInfo->display(routers);
  }
}

/** Display the network map window. If there are geoip requests waiting in the
 * queue, start the queue timers now. */
void
NetViewer::showWindow()
{
  if (!_resolveQueue.isEmpty()) {
    _minResolveQueueTimer.start(MIN_RESOLVE_QUEUE_DELAY);
    _maxResolveQueueTimer.start(MAX_RESOLVE_QUEUE_DELAY);
  }
  VidaliaWindow::showWindow();
}

/** Loads data into map, lists and starts timer when we get connected*/
void
NetViewer::onAuthenticated()
{
  _geoip.setSocksHost(_torControl->getSocksAddress(),
                      _torControl->getSocksPort());
  refresh();
  _refreshTimer.start();
  ui.actionRefresh->setEnabled(true);
}

/** Clears map, lists and stops timer when we get disconnected */
void
NetViewer::onDisconnected()
{
  clear();
  _refreshTimer.stop();
  ui.actionRefresh->setEnabled(false);
}

/** Custom event handler. Catches the new descriptor events. */
void
NetViewer::customEvent(QEvent *event)
{
  int type = event->type();
  
  if (type == CustomEventType::NewDescriptorEvent) {
    /* New router descriptor, so load it and add it to the list */
    NewDescriptorEvent *nde = (NewDescriptorEvent *)event;
    newDescriptors(nde->descriptorIDs());
  } else if (type == CustomEventType::CircuitEvent) {
    /* New or updated circuit information */
    CircuitEvent *ce = (CircuitEvent *)event;
    addCircuit(ce->circuit());
  } else if (type == CustomEventType::StreamEvent) {
    /* New or updated stream information */
    StreamEvent *se = (StreamEvent *)event;
    addStream(se->stream());
  } else if (type == CustomEventType::AddressMapEvent) {
    /* New or updated address mapping. We store the reverse of the new
     * mapping, so we can go from an IP address back to a hostname. */
    AddressMapEvent *ae = (AddressMapEvent *)event;
    _addressMap.add(ae->to(), ae->from(), ae->expires());
  }

  /* Update the world map */
  _map->update();
}

/** Reloads the lists of routers, circuits that Tor knows about */
void
NetViewer::refresh()
{
  /* Don't let the user refresh while we're refreshing. */
  ui.actionRefresh->setEnabled(false);

  /* Clear the data */
  clear();

  /* Load router information */
  loadNetworkStatus();
  /* Load existing address mappings */
  loadAddressMap();
  /* Load Circuits and Streams information */
  loadConnections();

  /* Ok, they can refresh again. */
  ui.actionRefresh->setEnabled(true);
} 

/** Clears the lists and the map */
void
NetViewer::clear()
{
  /* Clear the resolve queue and map */
  _resolveMap.clear();
  _resolveQueue.clear();
  /* Clear the network map */
  _map->clear();
  _map->update();
  /* Clear the address map */
  _addressMap.clear();
  /* Clear the lists of routers, circuits, and streams */
  ui.treeRouterList->clearRouters();
  ui.treeCircuitList->clearCircuits();
  ui.textRouterInfo->clear();
}

/** Loads a list of all current address mappings. */
void
NetViewer::loadAddressMap()
{
  /* We store the reverse address mappings, so we can go from a numeric value
   * back to a likely more meaningful hostname to display for the user. */
  _addressMap = _torControl->getAddressMap().reverse();
}

/** Loads a list of all current circuits and streams. */
void
NetViewer::loadConnections()
{
  /* Load all circuits */
  CircuitList circuits = _torControl->getCircuits();
  foreach (Circuit circuit, circuits) {
    addCircuit(circuit);
  }
  /* Now load all streams */
  StreamList streams = _torControl->getStreams();
  foreach (Stream stream, streams) {
    addStream(stream);
  }

  /* Update the map */
  _map->update();
}

/** Adds <b>circuit</b> to the map and the list */
void
NetViewer::addCircuit(const Circuit &circuit)
{
  /* Add the circuit to the list of all current circuits */
  ui.treeCircuitList->addCircuit(circuit);
  /* Plot the circuit on the map */
  _map->addCircuit(circuit.id(), circuit.routerIDs());
}

/** Adds <b>stream</b> to its associated circuit on the list of all circuits. */
void
NetViewer::addStream(const Stream &stream)
{
  QString target = stream.targetAddress();
  QHostAddress addr(target);
  
  /* If the stream's target has an IP address instead of a host name,
   * check our cache for an existing reverse address mapping. */
  if (!addr.isNull() && _addressMap.isMapped(target)) {
    /* Replace the IP address in the stream event with the original 
     * hostname */
    ui.treeCircuitList->addStream(
      Stream(stream.id(), stream.status(), stream.circuitId(),
             _addressMap.mappedTo(target), stream.targetPort()));
  } else {
    ui.treeCircuitList->addStream(stream);
  }
}

/** Called when the user selects the "Help" action from the toolbar. */
void
NetViewer::help()
{
  emit helpRequested("netview");
}

/** Retrieves a list of all running routers from Tor and their descriptors,
 * and adds them to the RouterListWidget. */
void
NetViewer::loadNetworkStatus()
{
  NetworkStatus networkStatus = _torControl->getNetworkStatus();
  foreach (RouterStatus rs, networkStatus) {
    if (!rs.isRunning())
      continue;

    RouterDescriptor rd = _torControl->getRouterDescriptor(rs.id());
    if (!rd.isEmpty())
      addRouter(rd);
  }
}

/** Adds a router to our list of servers and retrieves geographic location
 * information for the server. */
void
NetViewer::addRouter(const RouterDescriptor &rd)
{
  /* Add the descriptor to the list of server */
  ui.treeRouterList->addRouter(rd);

  /* Add this IP to a list of addresses whose geographic location we'd like to
   * find, but not for special purpose descriptors (e.g., bridges). This
   * check is only valid for Tor >= 0.2.0.13-alpha. */
  if (_torControl->getTorVersion() >= 0x020013) {
    DescriptorAnnotations annotations =
      _torControl->getDescriptorAnnotations(rd.id());
    if (!annotations.contains("purpose"))
      addToResolveQueue(rd.ip(), rd.id());
  } else {
    addToResolveQueue(rd.ip(), rd.id());
  }
}

/** Called when a NEWDESC event arrives. Retrieves new router descriptors
 * for the router identities given in <b>ids</b> and updates the router
 * list and network map. */
void
NetViewer::newDescriptors(const QStringList &ids)
{
  foreach (QString id, ids) {
    RouterDescriptor rd = _torControl->getRouterDescriptor(id);
    if (!rd.isEmpty())
      addRouter(rd); /* Updates the existing entry */
  }
}

/** Adds an IP address to the resolve queue and updates the queue timers. */
void
NetViewer::addToResolveQueue(const QHostAddress &ip, const QString &id)
{
  QString ipstr = ip.toString();
  if (!_resolveMap.values(ipstr).contains(id)) {
    /* Remember which server ids belong to which IP addresses */
    _resolveMap.insertMulti(ipstr, id);
  }
 
  if (!_resolveQueue.contains(ip) && !_geoip.resolveFromCache(ip)) {
    /* Add the IP to the queue of IPs waiting for geographic information  */
    _resolveQueue << ip;
 
    /* Wait MIN_RESOLVE_QUEUE_DELAY after the last item inserted into the
     * queue, before sending the resolve request. */
    _minResolveQueueTimer.start(MIN_RESOLVE_QUEUE_DELAY);
    
    /* Do not wait longer than MAX_RESOLVE_QUEUE_DELAY from the time the first
     * item is inserted into the queue, before flushing and resolving the
     * queue. */
    if (_resolveQueue.size() == 1) {
      _maxResolveQueueTimer.start(MAX_RESOLVE_QUEUE_DELAY);
    }
  }
}

/** Called when the user selects a circuit from the circuit and streams
 * list. */
void
NetViewer::circuitSelected(const Circuit &circuit)
{
  /* Clear any selected items. */
  ui.treeRouterList->deselectAll();
  ui.textRouterInfo->clear();
  _map->deselectAll();

  /* Select the items on the map and in the list */
  _map->selectCircuit(circuit.id());

  QList<RouterDescriptor> routers;

  foreach (QString id, circuit.routerIDs()) {
    /* Try to find and select each router in the path */
    RouterListItem *item = ui.treeRouterList->findRouterById(id);
    if (item)
      routers.append(item->descriptor());
  }

  ui.textRouterInfo->display(routers);
}

/** Called when the user selects one or more routers from the router list. */
void
NetViewer::routerSelected(const QList<RouterDescriptor> &routers)
{
  _map->deselectAll();
  ui.textRouterInfo->clear();
  ui.textRouterInfo->display(routers);

  /* XXX: Ideally we would also be able to select multiple pinpoints on the
   *      map. But our current map sucks and you can't even tell when one is
   *      selected anyway. Worry about this when we actually get to Marble.
   */
  if (routers.size() == 1)
    _map->selectRouter(routers[0].id());
}

/** If there are any IPs in the resolve queue, do the request now. */
void
NetViewer::resolve()
{
  if (!_resolveQueue.isEmpty()) {
    /* Send the request now if either the network map is visible, or the
     * request is for more than a quarter of the servers in the list. */
    if (isVisible() || 
        (_resolveQueue.size() >= ui.treeRouterList->topLevelItemCount()/4)) {
      vInfo("Sending GeoIP request for %1 IP addresses.")
                               .arg(_resolveQueue.size());
      /* Flush the resolve queue and stop the timers */
      _geoip.resolve(_resolveQueue);
      _resolveQueue.clear();
    }
  }
  /* Stop the queue timers. Only one should be active since the other is what
   * called this slot, but calling stop() on a stopped timer does not hurt. */
  _minResolveQueueTimer.stop();
  _maxResolveQueueTimer.stop();
}

/** Called when a list of GeoIp information has been resolved. */
void
NetViewer::resolved(int id, const QList<GeoIp> &geoips)
{
  Q_UNUSED(id);

  QString ip;
  RouterListItem *router;
 
  foreach (GeoIp geoip, geoips) {
    /* Find all routers that are at this IP address */
    ip = geoip.ip().toString();
    QList<QString> ids = _resolveMap.values(ip);
    _resolveMap.remove(ip);
      
    /* Update their geographic location information with the results of this
     * GeoIP query. */
    foreach (QString id, ids) {
      router = ui.treeRouterList->findRouterById(id);
      if (router) {
        /* Save the location information in the descriptor */
        router->setLocation(geoip);
        /* Plot the router on the map */
        _map->addRouter(router->descriptor(), geoip);
      }
    }
  }

  /* Update the circuit lines */
  foreach (Circuit circuit, ui.treeCircuitList->circuits()) {
    _map->addCircuit(circuit.id(), circuit.routerIDs());
  }

  /* Repaint the map */
  _map->update();
}

/** Called when the user selects a router on the network map. Displays a 
 * dialog with detailed information for the router specified by
 * <b>id</b>.*/
void
NetViewer::displayRouterInfo(const QString &id)
{
  RouterInfoDialog dlg(this);

  /* Fetch the specified router's descriptor */
  QStringList rd = _torControl->getRouterDescriptorText(id);
  if (rd.isEmpty()) {
    VMessageBox::warning(this, tr("Relay Not Found"),
                         tr("No details on the selected relay are available."),
                         VMessageBox::Ok);
    return;
  }

  /* Fetch the router's network status information */
  RouterStatus rs = _torControl->getRouterStatus(id);

  dlg.setRouterInfo(rd, rs);

  /* Populate the UI with information learned from a previous GeoIP request */
  RouterListItem *item = ui.treeRouterList->findRouterById(id);
  if (item)
    dlg.setLocation(item->location());
  else
    dlg.setLocation(tr("Unknown"));

  dlg.exec();
}

/* XXX: The following zoomIn() and zoomOut() slots are a hack. MarbleWidget
 *      does have zoomIn() and zoomOut() slots to which we could connect the
 *      buttons, but these slots currently don't force a repaint. So to see
 *      the zoom effect, the user has to click on the map after clicking one
 *      of the zoom buttons. Instead, we use the zoomViewBy() method, which
 *      DOES force a repaint.
 */
/** Called when the user clicks the "Zoom In" button. */
void
NetViewer::zoomIn()
{
#if defined(USE_MARBLE)
  _map->zoomViewBy(40);
#else
  _map->zoomIn();
#endif
}

/** Called when the user clicks the "Zoom Out" button. */
void
NetViewer::zoomOut()
{
#if defined(USE_MARBLE)
  _map->zoomViewBy(-40);
#else
  _map->zoomOut();
#endif
}

/** Called when the user clicks "Full Screen" or presses Escape on the map.
 * Toggles the map between normal and a full screen viewing modes. */
void
NetViewer::toggleFullScreen()
{
  if (_map->isFullScreen()) {
    /* Disabling full screen mode. Put the map back in its container. */
    ui.gridLayout->addWidget(_map);
    _map->setWindowState(_map->windowState() & ~Qt::WindowFullScreen);
  } else {
    /* Enabling full screen mode. Remove the map from the QGridLayout
     * container and set its window state to full screen. */
    _map->setParent(0);
    _map->setWindowState(_map->windowState() | Qt::WindowFullScreen);
    _map->show();
  }
}

