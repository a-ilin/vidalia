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

/** Converts world space coordinates into map space coordinates */
QPointF
TorMapWidget::toMapSpace(float latitude, float longitude)
{
  float width = IMG_WIDTH - MAP_LEFT - MAP_RIGHT;
  float height = IMG_HEIGHT - MAP_TOP - MAP_BOTTOM;
  float deg = width / 360.0;

  float lat;
  float lon;
  lat = floor(longitude * (deg * lerp(abs(int(latitude)), plen))
	      + width/2 + MAP_TOP);
  
  if (lat < 0) {
    lon = floor((height/2) + (lerp(abs(int(lat)), pdfe) * (height/2))
		+ MAP_TOP);
  } else {
    lon = floor((height/2) - (lerp(abs(int(lat)), pdfe) * (height/2))
		+ MAP_TOP);
  }

  return QPointF(lat, lon);
}
  
/** Linearly interpolates using  the values in the Robinson projection table */
float
TorMapWidget::lerp(float input, float *table)
{
  int x = floor(input / 5);

  return ((table[x+1] - table[x]) /
	  (((x+1)*5) - (x*5))) * (input - x*5) + table[x];
}

/** Returns the minimum size of the widget */
QSize
TorMapWidget::minimumSizeHint() const
{
  return MIN_SIZE;
}
