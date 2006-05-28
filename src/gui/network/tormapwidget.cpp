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

#include "tormapwidget.h"

#define IMG_WORLD_MAP   ":/images/map/world-map.png"


/** Default constructor */
TorMapWidget::TorMapWidget(QWidget *parent)
: ZImageView(parent)
{
  QImage map(IMG_WORLD_MAP);
  setImage(map);
}

/** Adds a router to the map. */
void
TorMapWidget::addRouter(QString name, float latitude, float longitude)
{
  Q_UNUSED(name);
  Q_UNUSED(latitude);
  Q_UNUSED(longitude);
}

/** Adds a circuit to the map using the given ordered list of routers. */
void
TorMapWidget::addCircuit(Circuit circuit)
{
  QStringList hops = circuit.hops();
  for (int i = 0; i < hops.size()-1; i++) {
    QString fromNode = hops.at(i);
    QString toNode = hops.at(i+1);
    /* Plot a line between fromNode and toNode */
  }
}

/** Selects and highlights a router on the map. */
void
TorMapWidget::selectRouter(QString name)
{
  Q_UNUSED(name);
}

/** Selects and highlights a circuit on the map. */
void
TorMapWidget::selectCircuit(Circuit circuit)
{
  Q_UNUSED(circuit);
}

