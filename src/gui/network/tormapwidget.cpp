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
 * \file tormapwidget.cpp
 * \version $Id$
 */
#include <QStringList>
#include <cmath>
#include "tormapwidget.h"

#define IMG_WORLD_MAP   ":/images/map/world-map.png"

/** QPens to use for drawing different map elements */
#define PEN_ROUTER        QPen(QColor("#ff030d"), 1.0)
#define PEN_CIRCUIT       QPen(Qt::yellow, 0.5)
#define PEN_SELECTED      QPen(Qt::green, 2.0)

/** Size of the map image */
#define IMG_WIDTH       1000
#define IMG_HEIGHT      507

/** Border between the edge of the image and the actual map */
#define MAP_TOP         2
#define MAP_BOTTOM      2
#define MAP_RIGHT       5
#define MAP_LEFT        5

/** Map offset from zero longitude */
#define MAP_ORGIN       -10

/** Minimum allowable size for this widget */
#define MIN_SIZE        QSize(512,256)

/** Robinson projection table */
/** Length of the parallel of latitude */
float  plen[] = {
    1.0000, 0.9986, 0.9954, 0.9900,
    0.9822, 0.9730, 0.9600, 0.9427,
    0.9216, 0.8962, 0.8679, 0.8350,
    0.7986, 0.7597, 0.7186, 0.6732,
    0.6213, 0.5722, 0.5322
  };

/** Distance of corresponding parallel from equator */ 
float  pdfe[] = {
    0.0000, 0.0620, 0.1240, 0.1860,
    0.2480, 0.3100, 0.3720, 0.4340,
    0.4958, 0.5571, 0.6176, 0.6769,
    0.7346, 0.7903, 0.8435, 0.8936,
    0.9394, 0.9761, 1.0000
  };

/** Default constructor */
TorMapWidget::TorMapWidget(QWidget *parent)
: ZImageView(parent)
{
  QImage map(IMG_WORLD_MAP);
  setImage(map);
}

/** Destructor */
TorMapWidget::~TorMapWidget()
{
  clear();
}

/** Adds a router to the map. */
void
TorMapWidget::addRouter(QString name, float latitude, float longitude)
{
  QPointF routerCoord = toMapSpace(latitude, longitude);
  
  /* Add data the hash of known routers, and plot the point on the map */
  _routers.insert(name, new QPair<QPointF,bool>(routerCoord, false));
}

/** Adds a circuit to the map using the given ordered list of routers. */
void
TorMapWidget::addCircuit(Circuit circuit)
{
  QPainterPath *circPainterPath = new QPainterPath;
  QStringList hops = circuit.hops();
  
  /* Build the new circuit */
  for (int i = 0; i < hops.size()-1; i++) {
    QString fromNode = hops.at(i);
    QString toNode = hops.at(i+1);
   
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
  int circid = circuit.id();
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
}

/** Removes a circuit from the map. */
void
TorMapWidget::removeCircuit(Circuit circuit)
{
  int key = circuit.id();
  QPair<QPainterPath*,bool> *circ = _circuits.take(key);
  QPainterPath *circpath = circ->first;
  if (circpath) {
    delete circpath;
  }
}

/** Selects and highlights a router on the map. */
void
TorMapWidget::selectRouter(QString name)
{
  if (_routers.contains(name)) {
    QPair<QPointF, bool> *routerPair = _routers.value(name);
    routerPair->second = true;
  }
  repaint();
}

/** Selects and highlights a circuit on the map. */
void
TorMapWidget::selectCircuit(Circuit circuit)
{
  int circid = circuit.id();
  if (_circuits.contains(circid)) {
    QPair<QPainterPath*, bool> *circuitPair = _circuits.value(circid);
    circuitPair->second = true;
  }
  repaint();
}

/** Deselects any highlighted routers or circuits */
void
TorMapWidget::deselectAll()
{
  /* Deselect all router points */
  foreach (QString router, _routers.keys()) {
    QPair<QPointF,bool> *routerPair = _routers.value(router);
    routerPair->second = false;
  }
  /* Deselect all circuit paths */
  foreach (int circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *circuitPair = _circuits.value(circid);
    circuitPair->second = false;
  }
}

/** Clears the list of routers and removes all the data on the map */
void
TorMapWidget::clear()
{
  /* Clear out all the router points and free their memory */
  foreach (QString router, _routers.keys()) {
    delete _routers.take(router);
  }
  /* Clear out all the circuit paths and free their memory */
  foreach (int circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *circuitPair = _circuits.take(circid);
    delete circuitPair->first;
    delete circuitPair;
  }
}
  
/** Draws the routers and paths onto the map image. */
void
TorMapWidget::paintImage(QPainter *painter)
{
  painter->setRenderHint(QPainter::Antialiasing);
  
  /* Draw the router points */
  foreach(QString router, _routers.keys()) {
    QPair<QPointF,bool> *routerPair = _routers.value(router);
    painter->setPen((routerPair->second ? PEN_SELECTED : PEN_ROUTER)); 
    painter->drawPoint(routerPair->first);
  }
  /* Draw the circuit paths */
  foreach(int circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *circuitPair = _circuits.value(circid);
    painter->setPen((circuitPair->second ? PEN_SELECTED : PEN_CIRCUIT));
    painter->drawPath(*(circuitPair->first));
  }
}

/** Converts world space coordinates into map space coordinates */
QPointF
TorMapWidget::toMapSpace(float latitude, float longitude)
{
  float width = IMG_WIDTH - MAP_LEFT - MAP_RIGHT;
  float height = IMG_HEIGHT - MAP_TOP - MAP_BOTTOM;
  float deg = width / 360.0;
  longitude += MAP_ORGIN;

  float lat;
  float lon;
  
  lat = floor(longitude * (deg * lerp(abs(int(latitude)), plen))
	      + width/2 + MAP_LEFT);
  
  if (latitude < 0) {
    lon = floor((height/2) + (lerp(abs(int(latitude)), pdfe) * (height/2))
		+ MAP_TOP);
  } else {
    lon = floor((height/2) - (lerp(abs(int(latitude)), pdfe) * (height/2))
		+ MAP_TOP);
  }

  return QPointF(lat, lon);
}
  
/** Linearly interpolates using the values in the Robinson projection table */
float
TorMapWidget::lerp(float input, float *table)
{
  int x = int(floor(input / 5));

  return ((table[x+1] - table[x]) / 
	  (((x+1)*5) - (x*5))) * (input - x*5) + table[x];
}

/** Returns the minimum size of the widget */
QSize
TorMapWidget::minimumSizeHint() const
{
  return MIN_SIZE;
}

/** Zooms to fit all currently displayed circuits on the map. If there are no
 * circuits on the map, the viewport will be returned to its default position
 * (zoomed all the way out and centered). */
void
TorMapWidget::zoomToFit()
{
  QRectF rect = circuitBoundingBox();
  
  if (rect.isNull()) {
    /* If there are no circuits, zoom all the way out */
    resetZoomPoint();
    zoom(0.0);
  } else {
    /* Zoom in on the displayed circuits */
    float zoomLevel = 1.0 - qMin(rect.height()/(float)height(),
                                 rect.width()/(float)width());

    zoom(rect.center().toPoint(), zoomLevel);
  }
}

/** Computes a bounding box around all currently displayed circuit paths on
 * the map. */
QRectF
TorMapWidget::circuitBoundingBox()
{
  QRectF rect;

  /* Compute the union of bounding rectangles for all circuit paths */
  foreach (int circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *pair = _circuits.value(circid);
    QPainterPath *circuit = pair->first;
    rect = rect.unite(circuit->boundingRect());
  }
  return rect;
}

