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
 * \version $Id$
 * \brief Displays an image and allows zooming and panning
 */

#ifndef ZIMAGEVIEW_H
#define ZIMAGEVIEW_H

#include <QImage>
#include <QFrame>
#include <QPixmap>
#include <QWidget>


class ZImageView : public QWidget
{
  Q_OBJECT

public:
  /** Default constructor. */
  ZImageView(QWidget *parent = 0);
  /** Sets the displayed image. */
  void setImage(QImage& pixmap);
  
public slots:
  /** Resets the center zoom point back to the center of the viewport. */
  void resetZoomPoint();
  /** Sets the current zoom level to the given percent. */
  void zoom(float pct);
  /** Sets the current zoom level to the given percent and scrolls the window
   * to place the specified point in the middle. */
  void zoom(QPoint zoomAt, float pct);
  /** Zooms into the displayed image by 5% */
  void zoomIn();
  /** Zooms away from the displayed image by 5% */
  void zoomOut();

protected:
  /** Virtual method to let subclasses paint on the image before it's scaled. */
  virtual void paintImage(QPainter *painter) { Q_UNUSED(painter); }
  /** Updates the viewport and repaints the displayed image. */
  virtual void paintEvent(QPaintEvent*);
  /** Handles the user pressing a mouse button. */
  virtual void mousePressEvent(QMouseEvent* e);
  /** Handles the user releasing a mouse button. */
  virtual void mouseReleaseEvent(QMouseEvent* e);
  /** Handles the user moving the mouse. */
  virtual void mouseMoveEvent(QMouseEvent* e);
  /** Handles events where the widget is resized. */
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
  /** Redraws the scaled image in the viewport. */
  void drawScaledImage();
  
private:
  float _zoom;     /**< The current zoom level. */
  QImage _image;   /**< The displayed image. */
  float _padding;  /**< Amount of padding to use on the side of the image. */
  float _maxZoomFactor;  /**< Maximum amount to zoom into the image. */

  bool _mouseDown;  /**< Set to true when a mouse button is depressed. */
  int  _mouseX;     /**< The x-coordinate of the current mouse position. */
  int  _mouseY;     /**< The y-coordinate of the current mouse position. */
  
  QRect _view;      /**< The displayed viewport. */
  float _desiredX;  /**< The X value we desire (???). */
  float _desiredY;  /**< The Y value we desire (???). */
};

#endif

