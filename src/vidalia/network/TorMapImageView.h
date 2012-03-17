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
** \file TorMapImageView.h
** \brief Displays Tor servers and circuits on a map of the world
*/

#ifndef _TORMAPIMAGEVIEW_H
#define _TORMAPIMAGEVIEW_H

#include "ZImageView.h"
#include "GeoIpRecord.h"

#include "RouterDescriptor.h"
#include "Circuit.h"

#include <QHash>
#include <QPair>
#include <QPainter>
#include <QPainterPath>


class TorMapImageView : public ZImageView
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorMapImageView(QWidget *parent = 0);
  /** Destructor. */
  ~TorMapImageView();

  /** Plots the given router on the map using the given coordinates. */
  void addRouter(const RouterDescriptor &desc, const GeoIpRecord &geoip);
  /** Plots the given circuit on the map. */
  void addCircuit(const CircuitId &circid, const QStringList &path);
  /** Selects and hightlights a router on the map. */
  void selectRouter(const QString &id);
  /** Selects and highlights a circuit on the map. */
  void selectCircuit(const CircuitId &circid);
  /** Returns the minimum size of the widget */
  QSize minimumSizeHint() const;

public slots:
  /** Removes a circuit from the map. */
  void removeCircuit(const CircuitId &circid);
  /** Deselects all the highlighted circuits and routers */
  void deselectAll();
  /** Clears the known routers and removes all the data from the map */
  void clear();
  /** Zooms to fit all currently displayed circuits on the map. */
  void zoomToFit();
  /** Zoom to a particular router on the map. */
  void zoomToRouter(const QString &id);
  /** Zoom to the circuit on the map with the given <b>circid</b>. */
  void zoomToCircuit(const CircuitId &circid);

protected:
  /** Paints the current circuits and streams on the image. */
  virtual void paintImage(QPainter *painter);

private:
  /** Converts world space coordinates into map space coordinates */
  QPointF toMapSpace(float latitude, float longitude);
  /** Linearly interpolates using the values in the projection table */
  float lerp(float input, float *table);
  /** Computes a bounding box around all currently displayed circuit paths on
   * the map. */
  QRectF circuitBoundingBox();

  /** Stores map locations for tor routers */
  QHash<QString, QPair<QPointF,bool>* > _routers;
  /** Stores circuit information */
  QHash<CircuitId, QPair<QPainterPath *,bool>* > _circuits;
};

#endif

