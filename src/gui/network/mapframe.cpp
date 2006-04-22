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

#define MIN_SIZE      QSize(400, 294)
#define IMG_MAP       ":/images/map/world-map.png"

#include "mapframe.h"

/** Constructor.
 * \param parent The parent widget of this MapFrame object.
 */
MapFrame::MapFrame(QWidget *parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

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
  
  gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
  
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
}  

/** Performs the painting operations */
void
MapFrame::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glCallList(_mapList);
}

/** Loads the map images into a texture and returns a display list */
void
MapFrame::makeMapList()
{
  GLuint texture;
  texture = bindTexture(QPixmap(QString(IMG_MAP)),
                        GL_TEXTURE_2D); 

  int coords[4][2] = {{1024,512},
                      {0,512},
                      {0,0},
                      {1024,0}};  
  
  _mapList = glGenLists(1);
  glNewList(_mapList, GL_COMPILE);
  {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
      for (int i = 0; i < 4; ++i) {
        glTexCoord2d(i == 0 || i == 3, i == 0 || i == 1);
        glVertex2d((GLdouble) coords[i][0],
                   (GLdouble) coords[i][1]);
      }
    glEnd();      
  }
  glEndList();
}

/** Returns a hint as to the minimum size of this widget */
QSize
MapFrame::minimumSizeHint() const
{
  return MIN_SIZE;
}

