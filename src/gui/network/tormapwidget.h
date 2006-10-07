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
 * \file tormapwidget.h
 * \version $Id$
 * \brief Displays Tor servers and circuits on a map of the world
 */

#ifndef _TORMAPWIDGET_H
#define _TORMAPWIDGET_H

#include <QHash>
#include <QPair>
#include <QPainter>
#include <QPainterPath>

#include "zimageview.h"


class TorMapWidget : public ZImageView
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorMapWidget(QWidget *parent = 0);
  /** Destructor. */
  ~TorMapWidget();

  /** Plots the given router on the map using the given coordinates. */
  void addRouter(QString id, float latitude, float longitude);
  /** Plots the given circuit on the map. */
  void addCircuit(quint64 circid, QStringList path);
  /** Selects and hightlights a router on the map. */
  void selectRouter(QString id);
  /** Selects and highlights a circuit on the map. */
  void selectCircuit(quint64 circid);
  /** Returns the minimum size of the widget */
  QSize minimumSizeHint() const;

public slots:
  /** Removes a circuit from the map. */
  void removeCircuit(quint64 circid);
  /** Deselects all the highlighted circuits and routers */
  void deselectAll();
  /** Clears the known routers and removes all the data from the map */
  void clear();
  /** Zooms to fit all currently displayed circuits on the map. */
  void zoomToFit();
  /** Zoom to a particular router on the map. */
  void zoomToRouter(QString id);

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
  QHash<quint64, QPair<QPainterPath *,bool>* > _circuits;
};

#endif

