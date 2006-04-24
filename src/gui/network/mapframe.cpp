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
 * \version $Id: $
 */

#include <QtDebug>
#include <vidalia.h>

#include "mapframe.h"

/** A quarter of the image size seems a reasonable minimum size */
#define MIN_SIZE      QSize(250, 125)
/** Default to displaying the entire map in the view box */
#define DEF_VIEW      QRectF(0.0, 0.0, 1024.0, 512.0)
#define IMG_MAP       ":/images/map/world-map.png"

/** Constructor.
 * \param parent The parent widget of this MapFrame object.
 */
MapFrame::MapFrame(QWidget *parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
  /** Initialize the viewBox to the default */
  _viewBox = DEF_VIEW;
  _buttonPressed = false;
  _action = Move; 
}

/** Initializes OpenGL */
void
MapFrame::initializeGL()
{
  qglClearColor(Qt::white); 
  glEnable(GL_TEXTURE_2D);
  makeMapList();
}

/** Handles resizing the widget */
void
MapFrame::resizeGL(int w, int h)
{  
  glViewport (0, 0,
               (GLsizei) w,
               (GLsizei) h);
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
}  

/** Performs the painting operations */
void
MapFrame::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  setViewBox();
  glCallList(_mapList);
}

/** Loads the map images into a texture and returns a display list */
void
MapFrame::makeMapList()
{
  GLuint texture;
  texture = bindTexture(QPixmap(QString(IMG_MAP)),
                        GL_TEXTURE_2D); 
  
  GLdouble coords[4][2] = {{_viewBox.width(), _viewBox.height()},
                      {0, _viewBox.height()},
                      {0, 0},
                      {_viewBox.width(), 0}};  
  
  _mapList = glGenLists(1);
  glNewList(_mapList, GL_COMPILE);
  {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
      for (int i = 0; i < 4; ++i) {
        glTexCoord2d(i == 0 || i == 3, i == 0 || i == 1);
        glVertex2d(coords[i][0], coords[i][1]);
      }
    glEnd();      
  }
  glEndList();
}

/** Sets up the view so that the rectangle in _viewBox is displayed */
void
MapFrame::setViewBox()
{
  gluOrtho2D(_viewBox.x(), _viewBox.width(), _viewBox.y(), _viewBox.height());
}

/** Converts widget space cooridnates to map space coordinates */
QPointF
MapFrame::toMapSpace(const QPoint &pos) const
{
  return QPointF(pos.x() * (_viewBox.width() / (double) width()),
                _viewBox.height() - (pos.y() *
                                    (_viewBox.height() / (double) height())));
}

/** Handles mouse button press events */
void
MapFrame::mousePressEvent(QMouseEvent *event)
{
  _buttonPressed = true;

  switch (_action)
  {
    /* FIXME None of this stuff works correctly yet :( */
    case Move:
      // call move function;
      break;
    
    case ZoomIn:
      zoom(ZoomIn);
      break;

    case ZoomOut:
      zoom(ZoomOut);
      break;

    default:
      break;
  }
}

/** Handles zooming */
void
MapFrame::zoom(MapAction zoomDir)
{
  /* FIXME This is just test stuff. It doesn't do anything useful. */
  double dx = (zoomDir == ZoomIn ? 4 : -4);
  double dy = (zoomDir == ZoomIn ? 2 : -2);
 
  while (_buttonPressed) {
    if (zoomDir == ZoomOut) {
      /* Don't zoom out farther than default view */
      if (_viewBox.width() > DEF_VIEW.width() ||
          _viewBox.height() > DEF_VIEW.height()) {
        _viewBox = DEF_VIEW;
        return;
      }
    } else {
      /* Don't zoom in too far either */
      if ((_viewBox.x() > _viewBox.width() - 10) ||
          (_viewBox.y() > _viewBox.height() - 10)) {
        return;
      }
    }
    
    _viewBox.setX(_viewBox.x() + dx);
    _viewBox.setY(_viewBox.y() + dy);
    _viewBox.setWidth(_viewBox.width() - dx);
    _viewBox.setHeight(_viewBox.height() - dy);
    updateGL();
    Vidalia::processEvents();
  }
}

/** Handles mouse button release events */
void
MapFrame::mouseReleaseEvent(QMouseEvent *event)
{
  _buttonPressed = false;
}

/** Handles mouse movement events */
void
MapFrame::mouseMoveEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
}

/** Sets the current MapAction */
void
MapFrame::setAction(MapAction action)
{
  _action = action;
}

/** Returns the current MapAction */
MapFrame::MapAction
MapFrame::getAction()
{
  return _action;
}

/** Returns the minimum widget size */
QSize
MapFrame::minimumSizeHint() const
{
  return MIN_SIZE;
}

