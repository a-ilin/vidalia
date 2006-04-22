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
 * \version $Id: mapframe.h 699 2006-04-15 03:12:22Z hipplej $
 */

#ifndef _MAPFRAME_H
#define _MAPFRAME_H

#include <QGLWidget>
#include <QPixmap>

class MapFrame : public QGLWidget
{
  Q_OBJECT

public:
  /** Default constructor */
  MapFrame(QWidget* parent = 0);
  /** Returns minimum allowable size */
  QSize minimumSizeHint() const;

protected:
  /* OpenGL functions */
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);

  void makeMapList();
  
private:
  GLuint _mapList;
};

#endif

