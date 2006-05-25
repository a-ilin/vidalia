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
 * \file zimageview.h
 * \version $Id: netviewer.cpp 699 2006-04-15 03:12:22Z hipplej $
 */

#ifndef ZIMAGEVIEW_H
#define ZIMAGEVIEW_H

#include <QImage>
#include <QFrame>
#include <QPixmap>
#include <QWidget>

//=============================================================================
//
//=============================================================================
class ZImageView : public QWidget
{
  Q_OBJECT

public:
  ZImageView(QWidget *parent = 0);
  virtual ~ZImageView();
  float getZoom();
  int getZoomInt();
  QSize minimumSizeHint() const;

public slots:
  void setImage(QImage& pixmap);
  void zoom(float pct);
  void zoomInt(int pct);
  void zoomWidth();
  void zoomHeight();
  void resetZoomPoint();
  void zoomIn();
  void zoomOut();

protected:
  void paintNow();
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void resizeEvent(QResizeEvent* e);
  
  /** Update the viewport.  This will set _view to a region that,
   *  when copied from the image and scaled to the screen size, will
   *  show what is expected.  The _view may be larger in one or more
   *  directions than the image, and you must deal with the 
   *  non-overlapping regions. 
   *
   *  Returns the _zoom==0.0 viewport rect (the max) and the
   *  _zoom==1.0 viewport rect (the min). */
  QPair<QRect, QRect> updateViewport(int screendx=0, int screendy=0);
  void drawScaledImage();
  float clamp(const float value, const float min, const float max);

private:
  float _zoom;
  QImage _image;
  QImage _imageCopy;
  float _padding;
  float _maxZoomFactor;

  bool _mouseDown;
  int  _mouseX;
  int  _mouseY;
  
  QRect _view;
  float _desiredX;
  float _desiredY;
};

#endif

