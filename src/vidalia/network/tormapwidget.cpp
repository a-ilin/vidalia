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
#include <vidalia.h>

#include "tormapwidgetinputhandler.h"
#include "tormapwidgetpopupmenu.h"
#include "tormapwidget.h"

using namespace Marble;

/** QPens to use for drawing different map elements */
#define CIRCUIT_NORMAL_PEN      QPen(Qt::green,  2.0)
#define CIRCUIT_SELECTED_PEN    QPen(Qt::yellow, 3.0)


/** Default constructor */
TorMapWidget::TorMapWidget(QWidget *parent)
  : MarbleWidget(parent)
{
  setMapThemeId("earth/srtm/srtm.dgml");
  setShowScaleBar(false);
  setShowCrosshairs(false);
  setAnimationsEnabled(true);
  setCursor(Qt::OpenHandCursor);

  TorMapWidgetInputHandler *handler = new TorMapWidgetInputHandler();
  TorMapWidgetPopupMenu *popupMenu  = new TorMapWidgetPopupMenu(this);

  connect(handler, SIGNAL(featureClicked(QPoint,Qt::MouseButton)),
          popupMenu, SLOT(featureClicked(QPoint,Qt::MouseButton)));
  connect(popupMenu, SIGNAL(displayRouterInfo(QString)),
          this, SIGNAL(displayRouterInfo(QString)));

  /* We can't call setInputHandler() until MarbleWidget has called its
   * internal _q_initGui() method, which doesn't happen until a
   * QTimer::singleShot(0, this, SLOT(_q_initGui())) timer set in its
   * constructor times out. So force that event to process now. */ 
  vApp->processEvents(QEventLoop::ExcludeUserInputEvents
                        | QEventLoop::ExcludeSocketNotifiers);

  setInputHandler(handler);
}

/** Destructor */
TorMapWidget::~TorMapWidget()
{
  clear();
}

/** Adds a router to the map. */
void
TorMapWidget::addRouter(const RouterDescriptor &desc, const GeoIp &geoip)
{
  QString kml;
  qreal lon = geoip.longitude();
  qreal lat = geoip.latitude();
  quint64 bw;
  
  bw = qMin(desc.averageBandwidth(), desc.burstBandwidth());
  bw = qMin(bw, desc.observedBandwidth());

  kml.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             "<kml xmlns=\"http://earth.google.com/kml/2.0\">"
             "<Document>"
             "  <Style id=\"normalPlacemark\">"
             "    <IconStyle><Icon><href>:/images/icons/placemark-relay.png</href></Icon></IconStyle>"
             "  </Style>"
             );

  kml.append("<Placemark>");
  kml.append("<styleUrl>#normalPlacemark</styleUrl>");
  kml.append(QString("<name>%1</name>").arg(desc.name()));
  kml.append(QString("<description>%1</description>").arg(desc.id()));
  kml.append(QString("<role>1</role>"));
  kml.append(QString("<address>%1</address>").arg(geoip.toString()));
  kml.append(QString("<CountryNameCode>%1</CountryNameCode>").arg(geoip.country()));
  kml.append(QString("<pop>%1</pop>").arg(10 * bw));
  kml.append(QString("<Point>"
                     "  <coordinates>%1,%2</coordinates>"
                     "</Point>").arg(lon).arg(lat));
  kml.append("</Placemark>");
  kml.append("</Document></kml>");

  QString id = desc.id();
  addPlaceMarkData(kml, id);
  _routers.insert(id, GeoDataCoordinates(lon, lat, 0.0,
                                         GeoDataCoordinates::Degree));
}

/** Adds a circuit to the map using the given ordered list of router IDs. */
void
TorMapWidget::addCircuit(const CircuitId &circid, const QStringList &path)
{
  /* XXX: Is it better to do KML LineString-based circuit drawing here,
   *      instead of going with a QPainter-based approach? I gave it a brief
   *      try once but failed. It might be worth looking into harder if we
   *      want to make circuits selectable on the map too.
   */

  /* It doesn't make sense to draw a path of length less than two */
  if (path.size() < 2)
    return;

  if (_circuits.contains(circid)) {
    /* Extend an existing path */
    CircuitGeoPath *geoPath = _circuits.value(circid);

    QString router = path.at(path.size()-1);
    if (_routers.contains(router)) {
      GeoDataCoordinates coords = _routers.value(router);
      geoPath->first.append(new GeoDataCoordinates(coords));
    }
  } else {
    /* Construct a new path */
    CircuitGeoPath *geoPath = new CircuitGeoPath();
    geoPath->second = false; /* initially unselected */

    foreach (QString router, path) {
      if (_routers.contains(router)) {
        GeoDataCoordinates coords = _routers.value(router);
        geoPath->first.append(new GeoDataCoordinates(coords));
      }      
    }
    _circuits.insert(circid, geoPath);
  }

  repaint();
}

/** Removes a circuit from the map. */
void
TorMapWidget::removeCircuit(const CircuitId &circid)
{
  CircuitGeoPath *path = _circuits.take(circid);
  if (path) {
    GeoDataLineString coords = path->first;
    qDeleteAll(coords.begin(), coords.end());
    delete path;
  }

  repaint();
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
  if (_circuits.contains(circid)) {
    CircuitGeoPath *path = _circuits.value(circid);
    path->second = true;
  }

  repaint();
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
#endif
  /* Deselect all circuit paths */
  foreach (CircuitGeoPath *path, _circuits.values()) {
    path->second = false;
  }

  repaint();
}

/** Clears the list of routers and removes all the data on the map */
void
TorMapWidget::clear()
{
  foreach (QString id, _routers.keys()) {
    removePlaceMarkKey(id);
  }

  foreach (CircuitId circid, _circuits.keys()) {
    CircuitGeoPath *path = _circuits.take(circid);
    GeoDataLineString coords = path->first;
    qDeleteAll(coords.begin(), coords.end());
    delete path;
  }

  repaint();
}
 
/** Zooms the map to fit entirely within the constraints of the current
 * viewport size. */
void
TorMapWidget::zoomToFit()
{
  int width  = size().width();
  int height = size().height();

  setRadius(qMin(width, height) / 2);

  /* XXX: Calling setRadius() seems to cause Marble to no longer draw the
   *      atmosphere. So, re-enable it. */
  setShowAtmosphere(true);
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
  if (_routers.contains(id)) {
    qreal lon, lat;
    GeoDataCoordinates coords = _routers.value(id);
    coords.geoCoordinates(lon, lat, GeoDataPoint::Degree);

    zoomView(maximumZoom());
    centerOn(lon, lat, true);
  }
}

/** Paints the current circuits and streams on the image. */
void
TorMapWidget::customPaint(GeoPainter *painter)
{
  foreach (CircuitGeoPath *path, _circuits.values()) {
    painter->setPen(path->second ? CIRCUIT_SELECTED_PEN : CIRCUIT_NORMAL_PEN);
    painter->drawPolyline(path->first);
  }
}

