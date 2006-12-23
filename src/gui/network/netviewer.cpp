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
 * \brief Displays a map of the Tor network and the user's circuits
 */

#include <QMessageBox>
#include <QHeaderView>
#include <vidalia.h>

#include "netviewer.h"

#define IMG_MOVE    ":/images/22x22/move-map.png"
#define IMG_ZOOMIN  ":/images/22x22/zoom-in.png"
#define IMG_ZOOMOUT ":/images/22x22/zoom-out.png"

/** Number of milliseconds to wait after the arrival of the last descriptor whose
 * IP needs to be resolved to geographic information, in case more descriptors
 * arrive. Then we can simply lump the IPs into a single request. */
#define MIN_RESOLVE_QUEUE_DELAY   3000
/** Maximum number of milliseconds to wait after the arrival of the first
 * IP address into the resolve queue, before we flush the entire queue. */
#define MAX_RESOLVE_QUEUE_DELAY   10000


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
#if !defined(Q_WS_WIN)
  ui.actionClose->setShortcut(QString("Ctrl+W"));
#endif

  /* Get the TorControl object */
  _torControl = Vidalia::torControl();
  _torControl->setEvent(TorEvents::NewDescriptor, this, true);
  _torControl->setEvent(TorEvents::CircuitStatus, this, true);
  _torControl->setEvent(TorEvents::StreamStatus,  this, true);
  
  /* Change the column widths of the tree widgets */
  ui.treeRouterList->header()->
    resizeSection(RouterListWidget::StatusColumn, 25);
  ui.treeRouterList->header()->
    resizeSection(RouterListWidget::CountryColumn, 25);
  ui.treeCircuitList->header()->
    resizeSection(CircuitListWidget::ConnectionColumn,
		  CircuitListWidget::ConnectionColumnWidth);

  /* Create the TorMapWidget and add it to the dialog */
  _map = new TorMapWidget();
  ui.gridLayout->addWidget(_map);

  /* Connect zoom buttons to ZImageView zoom slots */
  connect(ui.actionZoomIn, SIGNAL(triggered()), _map, SLOT(zoomIn()));
  connect(ui.actionZoomOut, SIGNAL(triggered()), _map, SLOT(zoomOut()));
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
  connect(ui.treeRouterList, SIGNAL(routerSelected(RouterDescriptor)),
	        this, SLOT(routerSelected(RouterDescriptor)));
  connect(ui.treeRouterList, SIGNAL(zoomToRouter(QString)),
          _map, SLOT(zoomToRouter(QString)));
  connect(ui.treeCircuitList, SIGNAL(circuitSelected(Circuit)),
          this, SLOT(circuitSelected(Circuit)));
  connect(ui.treeCircuitList, SIGNAL(circuitRemoved(quint64)),
          _map, SLOT(removeCircuit(quint64)));
  connect(ui.treeCircuitList, SIGNAL(zoomToCircuit(quint64)),
          _map, SLOT(zoomToCircuit(quint64)));
  connect(ui.treeCircuitList, SIGNAL(closeCircuit(quint64)),
          this, SLOT(closeCircuit(quint64)));
  connect(ui.treeCircuitList, SIGNAL(closeStream(quint64)),
          this, SLOT(closeStream(quint64)));

  /* Respond to changes in the status of the control connection */
  connect(_torControl, SIGNAL(connected(bool)), ui.actionRefresh, SLOT(setEnabled(bool)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(gotConnected()));
  connect(_torControl, SIGNAL(disconnected()), this, SLOT(gotDisconnected())); 

  /* Connect the slot to find out when geoip information has arrived */
  connect(&_geoip, SIGNAL(resolved(int, QList<GeoIp>)), 
             this,   SLOT(resolved(int, QList<GeoIp>)));
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

/** Clears map, lists and stops timer when we get disconnected */
void
NetViewer::gotDisconnected()
{
  clear();
  _refreshTimer.stop();
}

/** Loads data into map, lists and starts timer when we get connected*/
void
NetViewer::gotConnected()
{
  _geoip.setSocksHost(_torControl->getSocksAddress(),
                      _torControl->getSocksPort());
  refresh();
  _refreshTimer.start();
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
    addCircuit(ce->circuit());
  
  } else if (type == CustomEventType::StreamEvent) {
    /* New or updated stream information */
    StreamEvent *se = (StreamEvent *)event;
    ui.treeCircuitList->addStream(se->stream());
  }

  /** Update the map */
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
  loadDescriptors(_torControl->getRouterIDList());

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
  /* Clear the lists of routers, circuits, and streams */
  ui.treeRouterList->clearRouters();
  ui.treeCircuitList->clearCircuits();
  ui.textRouterInfo->clear();
}

/** Loads a list of all current circuits and streams. */
void
NetViewer::loadConnections()
{
  /* Load all circuits */
  QList<Circuit> circuits = _torControl->getCircuits();
  foreach (Circuit circuit, circuits) {
    addCircuit(circuit);
  }
  /* Now load all streams */
  QList<Stream> streams = _torControl->getStreams();
  foreach (Stream stream, streams) {
    ui.treeCircuitList->addStream(stream);
  }

  /* Update the map */
  _map->update();
}

/** Adds a circuit to the map and the list */
void
NetViewer::addCircuit(Circuit circuit)
{
  /* Circuits displayed to the user always use nicknames */
  Circuit circNames = circuitPathNames(circuit);
  /* Circuits on the map always use keyids */
  Circuit circIds   = circuitPathIDs(circuit);

  ui.treeCircuitList->addCircuit(circNames);
  _map->addCircuit(circuit.id(), circIds.hops());
}

/** Asks Tor to close the circuit with the id <b>circid</b>. */
void
NetViewer::closeCircuit(quint64 circid)
{
  _torControl->closeCircuit(circid);
}

/** Asks Tor to close the stream with the id <b>streamid</b>. */
void
NetViewer::closeStream(quint64 streamid)
{
  _torControl->closeStream(streamid);
}

/** Called when the user selects the "Help" action from the toolbar. */
void
NetViewer::help()
{
  Vidalia::help("netview");
}

/** Loads a list of new descriptors from the given IDs. */
void
NetViewer::loadDescriptors(QStringList ids)
{
  /* Get descriptors for all the given IDs */
  QList<RouterDescriptor> rds = _torControl->getDescriptorListById(ids);
   
  foreach (RouterDescriptor rd, rds) {
    /* Load the router descriptor and add it to the router list. */
    if (!rd.isEmpty()) {
      addRouter(rd);
    }
  }
}

/** Adds a router to our list of servers and retrieves geographic location
 * information for the server. */
void
NetViewer::addRouter(RouterDescriptor rd)
{
  /* Add the descriptor to the list of server */
  ui.treeRouterList->addRouter(rd);

  /* Add this IP to a list whose geographic location we'd like to find. */
  addToResolveQueue(rd.ip(), rd.id());
}

/** Adds an IP address to the resolve queue and updates the queue timers. */
void
NetViewer::addToResolveQueue(QHostAddress ip, QString id)
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
NetViewer::circuitSelected(Circuit circuit)
{
  /* Clear any selected items. */
  ui.treeRouterList->deselectAll();
  ui.textRouterInfo->clear();
  _map->deselectAll();

  /* Select the items on the map and in the list */
  _map->selectCircuit(circuit.id());

  QList<RouterDescriptor> routers;

  foreach (QString router, circuit.hops()) {
    /* Try to find and select each router in the path */
    RouterListItem *item = ui.treeRouterList->findRouterByName(router);
    if (item) {
      routers.append(item->descriptor());
    }
  }

  ui.textRouterInfo->display(routers);
}

/** Called when the user selects a router from the router list. */
void
NetViewer::routerSelected(RouterDescriptor router)
{
  _map->deselectAll();
  ui.textRouterInfo->clear();
  ui.textRouterInfo->display(router);
  _map->selectRouter(router.name());
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
NetViewer::resolved(int id, QList<GeoIp> geoips)
{
  Q_UNUSED(id);
  QString ip;
  RouterListItem *router;
  
  foreach (GeoIp geoip, geoips) {
    /* Find all routers that are at this IP address */
    ip = geoip.ip().toString();
    QList<QString> ids = _resolveMap.values(ip);
    _resolveMap.remove(ip);
    if (geoip.isUnknown())
      continue; /* We don't know where this router is */
      
    /* Update their geographic location information with the results of this
     * GeoIP query. */
    foreach (QString id, ids) {
      router = ui.treeRouterList->findRouterById(id);
      if (router) {
        /* Save the location information in the descriptor */
        router->setLocation(geoip);
        /* Plot the router on the map */
        _map->addRouter(router->id(), geoip.latitude(), geoip.longitude());
      }
    }
  }

  /* Update the circuit lines */
  foreach (Circuit circuit, ui.treeCircuitList->circuits()) {
    _map->addCircuit(circuit.id(), circuitPathIDs(circuit).hops());
  }
  
  /* Repaint the map */
  _map->update();
}

/** Convert all hops in <b>circ</b>'s path to server identities. <b>circ</b>'s
 * status and circuit ID will be preserved. If no ID can be found for a
 * router name, the name will be left in the circuit's path. */
Circuit
NetViewer::circuitPathIDs(Circuit circ)
{
  QStringList path = circ.hops();
  QStringList ids;
  RouterDescriptor rd;
  quint32 torVersion;

  torVersion = _torControl->getTorVersion();
  foreach (QString hop, path) {
    if (hop.startsWith("$")) {
      /* This hop is already specified as a keyid, so just strip the "$" */
      hop.remove(0,1);
    } else {
      if (torVersion < 0x00010202) {
        /* Tor versions earlier than 0.1.2.2 have a bug where they will tell
         * you a server's nickname in a circuit, even if that server is
         * non-Named. If we requested the descriptor by name for a non-Named server
         * from Tor, the user's log could be filled with warnings. So, just
         * try to look it up from our own router list first. */
        RouterListItem *item = ui.treeRouterList->findRouterByName(hop);
        if (item) {
          rd = item->descriptor();
        } else {
          /* We don't know about the router, so try to get it by name even
           * though Tor will probably yell at us. We can't help it. */
          rd = _torControl->getDescriptorByName(hop);
          if (!rd.isEmpty()) {
            /* Add this router to the list of those we know about */
            addRouter(rd);
          }
        }
      } else {
        /* Ask Tor for the descriptor for the given router name */
        rd = _torControl->getDescriptorByName(hop);
      }
      hop = (rd.isEmpty() ? hop : rd.id());
    }
    ids << hop;
  }
  return Circuit(circ.id(), circ.status(), ids);
}

/** Convert all hops in <b>circ</b>'s path to server names. <b>circ</b>'s
 * status and circuit ID will be preserved. If no name can be found for a
 * server identity, the ID will be left in the circuit's path. */
Circuit
NetViewer::circuitPathNames(Circuit circ)
{
  QStringList path = circ.hops();
  QStringList names;
  RouterDescriptor rd;
  
  foreach (QString hop, path) {
    if (hop.startsWith("$")) {
      /* Check if we already have a descriptor for this server identity. */
      RouterListItem *item =  ui.treeRouterList->findRouterById(hop.mid(1));
      if (item) {
        rd = item->descriptor();
      } else {
        /* A router with the current hop ID does not exist in our router list,
         * so ask Tor for its descriptor */
        rd = _torControl->getDescriptorById(hop);
        if (!rd.isEmpty()) {
          /* This is a fine time to add this new router to our list */
          addRouter(rd);
        }
      }
      hop = (rd.isEmpty() ? hop : rd.name());
    }
    names << hop;
  }
  return Circuit(circ.id(), circ.status(), names);
}

