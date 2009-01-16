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
** \file tormapwidget.h
** \version $Id$
** \brief Displays Tor servers and circuits on a map of the world
*/

#ifndef _TORMAPWIDGET_H
#define _TORMAPWIDGET_H

#include <QHash>
#include <QPair>
#include <QPainterPath>
#include <routerdescriptor.h>
#include <circuit.h>
#include <stream.h>

#include <MarbleWidget.h>
#include <GeoPainter.h>


class TorMapWidget : public Marble::MarbleWidget
{
  Q_OBJECT

public:
  /** Default constructor. */
  TorMapWidget(QWidget *parent = 0);
  /** Destructor. */
  ~TorMapWidget();

  /** Plots the given router on the map using the given coordinates. */
  void addRouter(const RouterDescriptor &desc,
                 float latitude, float longitude);
  /** Plots the given circuit on the map. */
  void addCircuit(const CircuitId &circid, const QStringList &path);
  /** Selects and hightlights a router on the map. */
  void selectRouter(const QString &id);
  /** Selects and highlights a circuit on the map. */
  void selectCircuit(const CircuitId &circid);

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
  virtual void customPaint(Marble::GeoPainter *painter);

private:
  /** Stores map locations for tor routers */
  QHash<QString, QPair<QPointF,bool>* > _routers;
  /** Stores circuit information */
  QHash<CircuitId, QPair<QPainterPath *,bool>* > _circuits;
};

#endif

