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
 * \file mapframe.cpp
 * \version $Id: mapframe.cpp 699 2006-04-15 03:12:22Z hipplej $
 */

#define MIN_SIZE      QSize(275, 153)
#define DEF_SIZE      QRect(QPoint(0,0), QSize(0,0))
#define IMG_MAP       ":/images/map/world-map.svg"

#include "mapframe.h"

#include <QtDebug>

/** Constructor.
 * \param parent The parent widget of this MapFrame object.
 */
MapFrame::MapFrame(QWidget *parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
  /* Create a new SvgRenderer object */
  _map = new QSvgRenderer(QString(IMG_MAP), this);

  if (!_map->isValid()) {
    qDebug() << QString("Invalid image. Fix it.");
  }

  connect(_map, SIGNAL(repaintNeeded()), this, SLOT(update()));
}

/** Overloaded QWidget::paintEvent(). Handles painting this widget */
void
MapFrame::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  _map->render(&painter);
}

/** Handles mouse button events. Start dragging */
void
MapFrame::mousePressEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
}

/** Handles mouse movement events. Scrolling */
void
MapFrame::mouseMoveEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
}

/** Handles mouse wheel events. Controls zoom in/out */
void
MapFrame::wheelEvent(QWheelEvent *event)
{
  const double diff = 0.25;
  QSize size = _map->defaultSize();
  int width  = size.width();
  int height = size.height();
  QRect currBox = _map->viewBox();
  
  qDebug() << QString("ViewBox: (%1 %2 %3 %4)").arg(currBox.x())
                                               .arg(currBox.y())
                                               .arg(currBox.width())
                                               .arg(currBox.height());
  
  if (event->delta() > 0) {
    width = int(this->width() + this->width() * diff);
    height = int(this->height() + this->height() * diff);
  } else {
    width = int(this->width() - this->width() * diff);
    height = int(this->height() - this->height() * diff);
  }
  resize(width, height);
}

QSize
MapFrame::minimumSizeHint() const
{
  return MIN_SIZE;
}

