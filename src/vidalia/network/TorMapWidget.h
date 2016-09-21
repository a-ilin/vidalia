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
** \file TorMapWidget.h
** \brief Displays Tor servers and circuits on a map of the world
*/

#ifndef _TORMAPWIDGET_H
#define _TORMAPWIDGET_H

#include "RouterDescriptor.h"
#include "GeoIpRecord.h"

#include "Circuit.h"
#include "Stream.h"

#include <MarbleWidget.h>
#include <GeoPainter.h>
#include <GeoDataCoordinates.h>
#include <GeoDataLineString.h>

#include <QHash>
#include <QPair>
#include <QPainterPath>

namespace Marble
{
  class GeoDataDocument;
  class GeoDataFolder;
  class GeoDataLineString;
}

typedef QPair<Marble::GeoDataLineString, bool> CircuitGeoPath;

class TorMapWidget : public Marble::MarbleWidget
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorMapWidget(QWidget *parent = 0);
  /** Destructor. */
  ~TorMapWidget();

  /** Plots the given router on the map using the given coordinates. */
  void addRouter(const RouterDescriptor &desc, const GeoIpRecord &geoip);
  /** Plots the given circuit on the map. */
  void addCircuit(const CircuitId &circid, const QStringList &path);
  /** Selects and highlights a circuit on the map. */
  void selectCircuit(const CircuitId &circid);

public slots:
  /** Removes a circuit from the map. */
  void removeCircuit(const CircuitId &circid);
  /** Deselects all the highlighted circuits and routers */
  void deselectAll();
  /** Clears the known routers and removes all the data from the map */
  void clear();
  /** Zooms the map to fit entirely within the constraints of the current
   * viewport size. */
  void zoomToFit();
  /** Zoom to a particular router on the map. */
  void zoomToRouter(const QString &id);
  /** Zoom to the circuit on the map with the given <b>circid</b>. */
  void zoomToCircuit(const CircuitId &circid);

signals:
  /** Emitted when the user selects a router placemark on the map. <b>id</b>
   * contain's the selected router's fingerprint. */
  void displayRouterInfo(const QString &id);

protected:
  /** Paints the current circuits and streams on the image. */
  virtual void customPaint(Marble::GeoPainter *painter);

private:
  /** Stores placemark IDs for Tor routers. */
  QHash<QString, Marble::GeoDataCoordinates> _routers;
  /** Stores circuit information */
  QHash<CircuitId, CircuitGeoPath*> _circuits;

  Marble::GeoDataDocument *_document;
  Marble::GeoDataFolder *_folder;
};

#endif

