/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file TorMapImageView.cpp
** \brief Displays Tor servers and circuits on a map of the world
*/

#include "config.h"
#include "TorMapImageView.h"

#include <QStringList>

#if defined(__sgi) && defined(_COMPILER_VERSION) && _COMPILER_VERSION >= 730
#include <math.h>
#else
#include <cmath>
#endif

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
#define MAP_WIDTH       (IMG_WIDTH-MAP_LEFT-MAP_RIGHT)
#define MAP_HEIGHT      (IMG_HEIGHT-MAP_TOP-MAP_BOTTOM)

/** Map offset from zero longitude */
#define MAP_ORIGIN       -10

/** Minimum allowable size for this widget */
#define MIN_SIZE        QSize(512,256)

/** Robinson projection table */
/** Length of the parallel of latitude */
static float  plen[] = {
    1.0000, 0.9986, 0.9954, 0.9900,
    0.9822, 0.9730, 0.9600, 0.9427,
    0.9216, 0.8962, 0.8679, 0.8350,
    0.7986, 0.7597, 0.7186, 0.6732,
    0.6213, 0.5722, 0.5322
  };

/** Distance of corresponding parallel from equator */
static float  pdfe[] = {
    0.0000, 0.0620, 0.1240, 0.1860,
    0.2480, 0.3100, 0.3720, 0.4340,
    0.4958, 0.5571, 0.6176, 0.6769,
    0.7346, 0.7903, 0.8435, 0.8936,
    0.9394, 0.9761, 1.0000
  };

/** Default constructor */
TorMapImageView::TorMapImageView(QWidget *parent)
: ZImageView(parent)
{
  QImage map(IMG_WORLD_MAP);
  setImage(map);
}

/** Destructor */
TorMapImageView::~TorMapImageView()
{
  clear();
}

/** Adds a router to the map. */
void
TorMapImageView::addRouter(const RouterDescriptor &desc, const GeoIpRecord &geoip)
{
  QString id = desc.id();
  QPointF routerCoord = toMapSpace(geoip.latitude(), geoip.longitude());

  /* Add data the hash of known routers, and plot the point on the map */
  if (_routers.contains(id))
    _routers.value(id)->first = routerCoord;
  else
    _routers.insert(id, new QPair<QPointF,bool>(routerCoord, false));
}

/** Adds a circuit to the map using the given ordered list of router IDs. */
void
TorMapImageView::addCircuit(const CircuitId &circid, const QStringList &path)
{
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
}

/** Removes a circuit from the map. */
void
TorMapImageView::removeCircuit(const CircuitId &circid)
{
  QPair<QPainterPath*,bool> *circ = _circuits.take(circid);
  QPainterPath *circpath = circ->first;
  if (circpath) {
    delete circpath;
  }
  delete circ;
}

/** Selects and highlights the router on the map. */
void
TorMapImageView::selectRouter(const QString &id)
{
  if (_routers.contains(id)) {
    QPair<QPointF, bool> *routerPair = _routers.value(id);
    routerPair->second = true;
  }
  repaint();
}

/** Selects and highlights the circuit with the id <b>circid</b>
 * on the map. */
void
TorMapImageView::selectCircuit(const CircuitId &circid)
{
  if (_circuits.contains(circid)) {
    QPair<QPainterPath*, bool> *circuitPair = _circuits.value(circid);
    circuitPair->second = true;
  }
  repaint();
}

/** Deselects any highlighted routers or circuits */
void
TorMapImageView::deselectAll()
{
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
}

/** Clears the list of routers and removes all the data on the map */
void
TorMapImageView::clear()
{
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
}

/** Draws the routers and paths onto the map image. */
void
TorMapImageView::paintImage(QPainter *painter)
{
  painter->setRenderHint(QPainter::Antialiasing);

  /* Draw the router points */
  foreach(QString router, _routers.keys()) {
    QPair<QPointF,bool> *routerPair = _routers.value(router);
    painter->setPen((routerPair->second ? PEN_SELECTED : PEN_ROUTER));
    painter->drawPoint(routerPair->first);
  }
  /* Draw the circuit paths */
  foreach(CircuitId circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *circuitPair = _circuits.value(circid);
    painter->setPen((circuitPair->second ? PEN_SELECTED : PEN_CIRCUIT));
    painter->drawPath(*(circuitPair->first));
  }
}

/** Converts world space coordinates into map space coordinates */
QPointF
TorMapImageView::toMapSpace(float latitude, float longitude)
{
  float width  = MAP_WIDTH;
  float height = MAP_HEIGHT;
  float deg = width / 360.0;
  longitude += MAP_ORIGIN;

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
TorMapImageView::lerp(float input, float *table)
{
  int x = int(floor(input / 5));

  return ((table[x+1] - table[x]) /
          (((x+1)*5) - (x*5))) * (input - x*5) + table[x];
}

/** Returns the minimum size of the widget */
QSize
TorMapImageView::minimumSizeHint() const
{
  return MIN_SIZE;
}

/** Zooms to fit all currently displayed circuits on the map. If there are no
 * circuits on the map, the viewport will be returned to its default position
 * (zoomed all the way out and centered). */
void
TorMapImageView::zoomToFit()
{
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
}

/** Zoom to the circuit on the map with the given <b>circid</b>. */
void
TorMapImageView::zoomToCircuit(const CircuitId &circid)
{
  if (_circuits.contains(circid)) {
    QPair<QPainterPath*,bool> *pair = _circuits.value(circid);
    QRectF rect = ((QPainterPath *)pair->first)->boundingRect();
    if (!rect.isNull()) {
      float zoomLevel = 1.0 - qMax(rect.height()/float(MAP_HEIGHT),
                                   rect.width()/float(MAP_WIDTH));

      zoom(rect.center().toPoint(), zoomLevel+0.2);
    }
  }
}

/** Zooms in on the router with the given <b>id</b>. */
void
TorMapImageView::zoomToRouter(const QString &id)
{
  QPair<QPointF,bool> *routerPair;

  if (_routers.contains(id)) {
    deselectAll();
    routerPair = _routers.value(id);
    routerPair->second = true;  /* Set the router point to "selected" */
    zoom(routerPair->first.toPoint(), 1.0);
  }
}

/** Computes a bounding box around all currently displayed circuit paths on
 * the map. */
QRectF
TorMapImageView::circuitBoundingBox()
{
  QRectF rect;

  /* Compute the union of bounding rectangles for all circuit paths */
  foreach (CircuitId circid, _circuits.keys()) {
    QPair<QPainterPath*,bool> *pair = _circuits.value(circid);
    QPainterPath *circuit = pair->first;
    rect = rect.unite(circuit->boundingRect());
  }
  return rect;
}

