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
** \file tormapwidget.cpp
** \version $Id$
** \brief Displays Tor servers and circuits on a map of the world
*/

#include <QStringList>
#include <cmath>
#include "tormapwidget.h"

using namespace Marble;

/** QPens to use for drawing different map elements */
#define PEN_ROUTER        QPen(QColor("#ff030d"), 1.0)
#define PEN_CIRCUIT       QPen(Qt::yellow, 0.5)
#define PEN_SELECTED      QPen(Qt::green, 2.0)


/** Default constructor */
TorMapWidget::TorMapWidget(QWidget *parent)
  : MarbleWidget(parent)
{
  setMapThemeId("earth/srtm/srtm.dgml");
}

/** Destructor */
TorMapWidget::~TorMapWidget()
{
#if 0
  clear();
#endif
}

/** Adds a router to the map. */
void
TorMapWidget::addRouter(const QString &id, float latitude, float longitude)
{
#if 0
  QPointF routerCoord = toMapSpace(latitude, longitude);
  
  /* Add data the hash of known routers, and plot the point on the map */
  if (_routers.contains(id))
    _routers.value(id)->first = routerCoord;
  else
    _routers.insert(id, new QPair<QPointF,bool>(routerCoord, false));
#endif
}

/** Adds a circuit to the map using the given ordered list of router IDs. */
void
TorMapWidget::addCircuit(const CircuitId &circid, const QStringList &path)
{
#if 0
  QPainterPath *circPainterPath = new QPainterPath;
  
  /* Build the new circuit */
  for (int i = 0; i < path.size()-1; i++) {
    QString fromNode = path.at(i);
    QString toNode = path.at(i+1);
   
    /* Add the coordinates of the hops to the circuit */
    if (_routers.contains(fromNode) && _routers.contains(toNode)) {
      /* Find the two endpoints for this path segment */
      QPointF fromPos = _routers.value(fromNode)->first;
      QPointF endPos = _routers.value(toNode)->first;
      
      /* Draw the path segment */ 
      circPainterPath->moveTo(fromPos);
      circPainterPath->lineTo(endPos);
      circPainterPath->moveTo(endPos);
    }
  }
  
  /** Add the data to the hash of known circuits and plot the circuit on the map */
  if (_circuits.contains(circid)) {
    /* This circuit is being updated, so just update the path, making sure we
     * free the memory allocated to the old one. */
    QPair<QPainterPath*,bool> *circuitPair = _circuits.value(circid);
    delete circuitPair->first;
    circuitPair->first = circPainterPath;
  } else {
    /* This is a new path, so just add it to our list */
    _circuits.insert(circid, new QPair<QPainterPath*,bool>(circPainterPath,false));
  }
#endif
}

/** Removes a circuit from the map. */
void
TorMapWidget::removeCircuit(const CircuitId &circid)
{
#if 0
  QPair<QPainterPath*,bool> *circ = _circuits.take(circid);
  QPainterPath *circpath = circ->first;
  if (circpath) {
    delete circpath;
  }
  delete circ;
#endif
}

/** Selects and highlights the router on the map. */
void
TorMapWidget::selectRouter(const QString &id)
{
#if 0
  if (_routers.contains(id)) {
    QPair<QPointF, bool> *routerPair = _routers.value(id);
    routerPair->second = true;
  }
  repaint();
#endif
}

/** Selects and highlights the circuit with the id <b>circid</b> 
 * on the map. */
void
TorMapWidget::selectCircuit(const CircuitId &circid)
{
#if 0
  if (_circuits.contains(circid)) {
    QPair<QPainterPath*, bool> *circuitPair = _circuits.value(circid);
    circuitPair->second = true;
  }
  repaint();
#endif
}

/** Deselects any highlighted routers or circuits */
void
TorMapWidget::deselectAll()
{
#if 0
  /* Deselect all router points */
  foreach (QString router, _routers.keys()) {
    QPair<QPointF,bool> *routerPair = _routers.value(router);
    routerPair->second = false;
  }
  /* Deselect all circuit paths */
  foreach (CircuitId circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *circuitPair = _circuits.value(circid);
    circuitPair->second = false;
  }
#endif
}

/** Clears the list of routers and removes all the data on the map */
void
TorMapWidget::clear()
{
#if 0
  /* Clear out all the router points and free their memory */
  foreach (QString router, _routers.keys()) {
    delete _routers.take(router);
  }
  /* Clear out all the circuit paths and free their memory */
  foreach (CircuitId circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *circuitPair = _circuits.take(circid);
    delete circuitPair->first;
    delete circuitPair;
  }
#endif
}
 
/** Zooms to fit all currently displayed circuits on the map. If there are no
 * circuits on the map, the viewport will be returned to its default position
 * (zoomed all the way out and centered). */
void
TorMapWidget::zoomToFit()
{
#if 0
  QRectF rect = circuitBoundingBox();
  
  if (rect.isNull()) {
    /* If there are no circuits, zoom all the way out */
    resetZoomPoint();
    zoom(0.0);
  } else {
    /* Zoom in on the displayed circuits */
    float zoomLevel = 1.0 - qMax(rect.height()/float(MAP_HEIGHT),
                                 rect.width()/float(MAP_WIDTH));
    
    zoom(rect.center().toPoint(), zoomLevel+0.2);
  }
#endif
}

/** Zoom to the circuit on the map with the given <b>circid</b>. */
void
TorMapWidget::zoomToCircuit(const CircuitId &circid)
{
#if 0
  if (_circuits.contains(circid)) {
    QPair<QPainterPath*,bool> *pair = _circuits.value(circid);
    QRectF rect = ((QPainterPath *)pair->first)->boundingRect();
    if (!rect.isNull()) {
      float zoomLevel = 1.0 - qMax(rect.height()/float(MAP_HEIGHT),
                                   rect.width()/float(MAP_WIDTH));

      zoom(rect.center().toPoint(), zoomLevel+0.2);
    }
  }
#endif
}

/** Zooms in on the router with the given <b>id</b>. */
void
TorMapWidget::zoomToRouter(const QString &id)
{
#if 0
  QPair<QPointF,bool> *routerPair;
  
  if (_routers.contains(id)) {
    deselectAll();
    routerPair = _routers.value(id);
    routerPair->second = true;  /* Set the router point to "selected" */
    zoom(routerPair->first.toPoint(), 1.0); 
  }
#endif
}

