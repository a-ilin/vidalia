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
 * \file mapframe.h
 * \version $Id: $
 */

#ifndef _MAPFRAME_H
#define _MAPFRAME_H

#include <QGLWidget>
#include <QPixmap>
#include <QMouseEvent>

class MapFrame : public QGLWidget
{
  Q_OBJECT

public:
  /** MapAction types */
  enum MapAction { Move, ZoomIn, ZoomOut };
  
  /** Default constructor */
  MapFrame(QWidget* parent = 0);
  /** Returns the minimum widget size */
  QSize minimumSizeHint() const;
  /** Sets the current map action */
  void setAction(MapAction action);
  /** Returns the current map action */
  MapAction getAction();

protected:
  /** Initializes OpenGL. Gets called once on load only */
  void initializeGL();
  /** Gets called whenever the widget has been resized */
  void resizeGL(int w, int h);
  /** Gets called when the widget needs repainting. */
  void paintGL();
  /** Handles mouse button press events */
  void mousePressEvent(QMouseEvent *event);
  /** Handles mouse button release events */
  void mouseReleaseEvent(QMouseEvent *event);
  /** Handles mouse movement events */
  void mouseMoveEvent(QMouseEvent *event);  

  /** Creates an OpenGL display list containing the map texture */
  void makeMapList();
  
private:
  /** Sets up the view so that it displays the rectagle in _viewBox */
  void setViewBox();
  /** Converts widget space coordinates to map space coordinates */
  QPointF toMapSpace(const QPoint &pos) const;
  /** Handles zooming */
  void zoom(MapAction zoomDir);
  
  /** An OpenGL display list */
  GLuint _mapList;
  /** Current viewing box */
  QRectF _viewBox;
  /** Coordinates of last position */
  QPointF _lastPos;
  /** Indicates if the mouse button is down */
  bool _buttonPressed;
  /** The current map aciton */
  MapAction _action;
};

#endif

