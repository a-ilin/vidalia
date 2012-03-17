/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file ZImageView.h
** \brief Displays an image and allows zooming and panning
*/

#ifndef ZIMAGEVIEW_H
#define ZIMAGEVIEW_H

#include <QImage>
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
  /** Handles the user double-clicking a mouse button. */
  virtual void mouseDoubleClickEvent(QMouseEvent *e);
  /** Handles the wheel events. */
  virtual void wheelEvent(QWheelEvent *e);

  /** Update the viewport.  This will set _view to a region that,
   *  when copied from the image and scaled to the screen size, will
   *  show what is expected.  The _view may be larger in one or more
   *  directions than the image, and you must deal with the
   *  non-overlapping regions. */
  void updateViewport(int screendx=0, int screendy=0);
  /** Redraws the scaled image in the viewport. */
  void drawScaledImage();

private:
  float _zoom;     /**< The current zoom level. */
  QImage _image;   /**< The displayed image. */
  float _padding;  /**< Amount of padding to use on the side of the image. */
  float _maxZoomFactor;  /**< Maximum amount to zoom into the image. */

  int  _mouseX;     /**< The x-coordinate of the current mouse position. */
  int  _mouseY;     /**< The y-coordinate of the current mouse position. */

  QRect _view;      /**< The displayed viewport. */
  float _desiredX;  /**< The X value we desire (???). */
  float _desiredY;  /**< The Y value we desire (???). */
};

#endif

