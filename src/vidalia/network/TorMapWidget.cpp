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
** \file TorMapWidget.cpp
** \brief Displays Tor servers and circuits on a map of the world
*/

#include "TorMapWidget.h"
#include "TorMapWidgetPopupMenu.h"
#include "Vidalia.h"

#include <MarbleModel.h>
#include <MarbleWidgetInputHandler.h>
#include <GeoDataPlacemark.h>
#include <GeoDataTreeModel.h>
#include <GeoDataDocument.h>
#include <GeoDataFolder.h>
#include <HttpDownloadManager.h>

#include <QStringList>

using namespace Marble;

/** QPens to use for drawing different map elements */
#define CIRCUIT_NORMAL_PEN      QPen(QBrush(QColor(0,51,102)),  2.0)
#define CIRCUIT_SELECTED_PEN    QPen(QBrush(QColor(65,146,75)), 3.0)


/** Default constructor */
TorMapWidget::TorMapWidget(QWidget *parent)
  : MarbleWidget(parent)
{
  setMapThemeId("earth/srtm/srtm.dgml");
  setShowScaleBar(false);
  setShowCrosshairs(false);
  setAnimationsEnabled(true);
  setCursor(Qt::OpenHandCursor);

  model()->downloadManager()->setDownloadEnabled(false);

  _document = new Marble::GeoDataDocument();
  _folder = new Marble::GeoDataFolder();
  model()->treeModel()->addDocument(_document);
  model()->treeModel()->addFeature(_document, _folder);

  TorMapWidgetPopupMenu *popupMenu  = new TorMapWidgetPopupMenu(this);

  // Properly disable all right click menus
  inputHandler()->setMouseButtonPopupEnabled(Qt::RightButton, false);

  // Hack to disable Marble's menus
  disconnect(inputHandler(), SIGNAL(lmbRequest(int,int)),
             0,0);

  connect(inputHandler(), SIGNAL(lmbRequest(int,int)),
          popupMenu, SLOT(featureLeftClicked(int,int)));
  connect(popupMenu, SIGNAL(displayRouterInfo(QString)),
          this, SIGNAL(displayRouterInfo(QString)));
}

/** Destructor */
TorMapWidget::~TorMapWidget()
{
  clear();
}

/** Adds a router to the map. */
void
TorMapWidget::addRouter(const RouterDescriptor &desc, const GeoIpRecord &geoip)
{
  qreal lon = geoip.longitude();
  qreal lat = geoip.latitude();
  quint64 bw;

  bw = qMin(desc.averageBandwidth(), desc.burstBandwidth());
  bw = qMin(bw, desc.observedBandwidth());

  QString id = desc.id();

  GeoDataPlacemark *pm = new GeoDataPlacemark(desc.name());
  pm->setDescription(desc.id());
  pm->setRole("1");
  pm->setAddress(geoip.toString());
  pm->setCountryCode(geoip.country());
  pm->setPopularity(10 * bw);
  pm->setCoordinate(lon, lat, 0.0, GeoDataCoordinates::Degree);
  model()->treeModel()->addFeature(_folder, pm);
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
    if (_routers.contains(router))
      geoPath->first.append(_routers.value(router));
  } else {
    /* Construct a new path */
    CircuitGeoPath *geoPath = new CircuitGeoPath();
    geoPath->second = false; /* initially unselected */

    foreach (QString router, path) {
      if (_routers.contains(router))
        geoPath->first.append(_routers.value(router));
    }
    geoPath->first.setTessellationFlags(Tessellate | RespectLatitudeCircle);
    _circuits.insert(circid, geoPath);
  }

  repaint();
}

/** Removes a circuit from the map. */
void
TorMapWidget::removeCircuit(const CircuitId &circid)
{
  CircuitGeoPath *path = _circuits.take(circid);
  if (path)
    delete path;

  repaint();
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
    model()->removeGeoData(id);
  }

  foreach (CircuitId circid, _circuits.keys()) {
    CircuitGeoPath *path = _circuits.take(circid);
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
  if (_circuits.contains(circid)) {
    CircuitGeoPath *pair = _circuits.value(circid);
    centerOn(pair->first.latLonAltBox(), true);
  }
}

/** Zooms in on the router with the given <b>id</b>. */
void
TorMapWidget::zoomToRouter(const QString &id)
{
  if (_routers.contains(id)) {
    qreal lon, lat;
    GeoDataCoordinates coords = _routers.value(id);

    coords.geoCoordinates(lon, lat,
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                          GeoDataCoordinates::Degree);
#else
                          GeoDataPoint::Degree);
#endif

    zoomView(zoomFromDistance(1000));
    centerOn(lon, lat, true);
  }
}

/** Paints the current circuits and streams on the image. */
void
TorMapWidget::customPaint(GeoPainter *painter)
{
  bool selected = false;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  painter->autoMapQuality();
#endif

  painter->setPen(CIRCUIT_NORMAL_PEN);

  foreach (CircuitGeoPath *path, _circuits.values()) {
    if (! path->second && selected) {
      painter->setPen(CIRCUIT_NORMAL_PEN);
      selected = false;
    } else if (path->second && ! selected) {
      painter->setPen(CIRCUIT_SELECTED_PEN);
      selected = true;
    }
    painter->drawPolyline(path->first);
  }
}

