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
** \file ZImageView.cpp
** \brief Displays an image and allows zooming and panning
*/

#include "ZImageView.h"

#include <QPainter>
#include <QMouseEvent>

#include <cmath>

#if QT_VERSION >= 0x040200
#define CURSOR_NORMAL           QCursor(Qt::OpenHandCursor)
#define CURSOR_MOUSE_PRESS      QCursor(Qt::ClosedHandCursor)
#else
#define CURSOR_NORMAL           QCursor(Qt::CrossCursor)
#define CURSOR_MOUSE_PRESS      QCursor(Qt::SizeAllCursor)
#endif


/** Constructor. */
ZImageView::ZImageView(QWidget *parent)
  : QWidget(parent)
{
  /* Initialize members */
  _zoom = 0.0;
  _desiredX = 0.0;
  _desiredY = 0.0;
  _maxZoomFactor = 2.0;
  _padding = 60;

  setCursor(CURSOR_NORMAL);
  updateViewport();
  resetZoomPoint();
  repaint();
}

/** Sets the displayed image. */
void
ZImageView::setImage(QImage& img)
{
  _image = img.copy();
  updateViewport();
  resetZoomPoint();

  if (isVisible()) {
    repaint();
  }
}

/** Draws the scaled image on the widget. */
void
ZImageView::drawScaledImage()
{
  if (!isVisible()) {
    return;
  }

  QBrush background(QColor("#fdfdfd"));
  if (_image.isNull()) {
    QPainter p(this);
    p.fillRect(rect(), background);
    return;
  }

  QRect sRect = rect();
  QRect iRect = _image.rect();
  QRect r = _view;

  // Think of the _view as being overlaid on the image.  The _view has the same
  // aspect ratio as the screen, so we cut the _view region out of the _image
  // and scale it to the screen dimensions and paint it.

  // There is a slight catch in that the _view may be larger than the image in
  // one or both directions.  In that case, we need to reduce the _view region
  // to lie within the image, then paint the background around it.  Copying
  // a region from an image where the region is bigger than the image results
  // in the parts outside the image being black, which is not what we want.

  // The view has the same aspect ratio as the screen, so the vertical and
  // horizontal scale factors will be equal.

  double scaleFactor = double(sRect.width()) / double(_view.width());

  // Constrain r to lie entirely within the image.
  if (r.top() < 0) {
    r.setTop(0);
  }
  if (iRect.bottom() < r.bottom()) {
    r.setBottom(iRect.bottom());
  }
  if (r.left() < 0) {
    r.setLeft(0);
  }
  if (iRect.right() < r.right()) {
    r.setRight(iRect.right());
  }

  // Figure out the size that the 'r' region will be when drawn to the screen.
  QSize scaleTo(int(double(r.width()) * scaleFactor),
		int(double(r.height()) * scaleFactor));

  /** Make a copy of the image so we don't ruin the original */
  QImage i = _image.copy();

  /** Create a QPainter that draws directly on the copied image and call the
   * virtual function to draw whatever the subclasses need to on the image. */
  QPainter painter;
  painter.begin(&i);
  paintImage(&painter);
  painter.end();

  /** Rescale the image copy */
  i = i.copy(r).scaled(scaleTo,
		     Qt::KeepAspectRatioByExpanding,
		     Qt::SmoothTransformation);

  int extraWidth = int(double(sRect.width() - i.width()) / 2.0);
  int extraHeight = int(double(sRect.height() - i.height()) / 2.0);

  // We don't want to paint the background
  // because this isn't double buffered and that would flicker.
  // We could double buffer it, but that would cost ~3 MB of memory.

  QPainter p(this);
  if (extraWidth > 0) {
    p.fillRect(0, 0, extraWidth, sRect.height(), background);
    p.fillRect(sRect.width() - extraWidth, 0,
	       sRect.width(), sRect.height(), background);
  }

  if (extraHeight > 0) {
    p.fillRect(0, 0, sRect.width(), extraHeight, background);
    p.fillRect(0, sRect.height() - extraHeight,
	       sRect.width(), sRect.height(), background);
  }

  // Finally, paint the image copy.
  p.drawImage(extraWidth, extraHeight, i);
}

/** Updates the displayed viewport. */
void
ZImageView::updateViewport(int screendx, int screendy)
{
  /* The gist of this is to find the biggest and smallest possible viewports,
   * then use the _zoom factor to interpolate between them.  Also pan the
   * viewport, but constrain each dimension to lie within the image or to be
   * centered if the image is too small in that direction. */

  QRect sRect = rect();
  QRect iRect = _image.rect();

  float sw = float(sRect.width());
  float sh = float(sRect.height());
  float iw = float(iRect.width());
  float ih = float(iRect.height());

  // Get the initial max and min sizes for the viewport.  These won't have the
  // correct aspect ratio.  They will actually be the least upper bound and
  // greatest lower bound of the set containing the screen and image rects.
  float maxw = float(std::max<int>(sRect.width(), iRect.width())) + _padding;
  float maxh = float(std::max<int>(sRect.height(), iRect.height())) + _padding;
  float minw = std::ceil(float(sRect.width()) / _maxZoomFactor);
  float minh = std::ceil(float(sRect.height()) / _maxZoomFactor);

  // Now that we have the glb and the lub, we expand/shrink them until
  // the aspect ratio is that of the screen.
  float aspect = sw / sh;

  // Fix the max rect.
  float newmaxh = maxh;
  float newmaxw = aspect * newmaxh;
  if (newmaxw < maxw) {
    newmaxw = maxw;
    newmaxh = maxw / aspect;
  }

  // Fix the min rect.
  float newminh = minh;
  float newminw = aspect * newminh;
  if (minw < newminw) {
    newminw = minw;
    newminh = newminw / aspect;
  }

  // Now interpolate between max and min.
  float vw = (1.0f - _zoom) * (newmaxw - newminw) + newminw;
  float vh = (1.0f - _zoom) * (newmaxh - newminh) + newminh;

  _view.setWidth(int(vw));
  _view.setHeight(int(vh));

  // Now pan the view

  // Convert the pan delta from screen coordinates to view coordinates.
  float vdx = vw * (float(screendx) / sw);
  float vdy = vh * (float(screendy) / sh);

  // Constrain the center of the viewport to the image rect.
  _desiredX = qBound(0.0f, _desiredX + vdx, iw);
  _desiredY = qBound(0.0f, _desiredY + vdy, ih);
  _view.moveCenter(QPoint(int(_desiredX), int(_desiredY)));

  QPoint viewCenter = _view.center();
  float vx = viewCenter.x();
  float vy = viewCenter.y();

  // The viewport may be wider than the height and/or width.  In that case,
  // center the view over the image in the appropriate directions.
  //
  // If the viewport is smaller than the image in either direction, then make
  // sure the edge of the viewport isn't past the edge of the image.

  vdx = 0;
  vdy = 0;

  if (iw <= vw) {
    vdx = (iw / 2.0f) - vx;  // Center horizontally.
  } else {
    // Check that the edge of the view isn't past the edge of the image.
    float vl = float(_view.left());
    float vr = float(_view.right());
    if (vl < 0) {
      vdx = -vl;
    } else if (vr > iw) {
      vdx = iw - vr;
    }
  }

  if (ih <= vh) {
    vdy = (ih / 2.0f) - vy; // Center vertically.
  } else {
    // Check that the edge of the view isn't past the edge of the image.
    float vt = float(_view.top());
    float vb = float(_view.bottom());
    if (vt < 0) {
      vdy = -vt;
    } else if (vb > ih) {
      vdy = ih - vb;
    }
  }

  _view.translate(int(vdx), int(vdy));
}

/** Resets the zoom point back to the center of the viewport. */
void
ZImageView::resetZoomPoint()
{
  QPoint viewCenter = _view.center();
  _desiredX = viewCenter.x();
  _desiredY = viewCenter.y();
}

/** Handles repainting this widget by updating the viewport and drawing the
 * scaled image. */
void
ZImageView::paintEvent(QPaintEvent*)
{
  updateViewport();
  drawScaledImage();
}

/** Sets the current zoom percentage to the given value and scrolls the
 * viewport to center the given point. */
void
ZImageView::zoom(QPoint zoomAt, float pct)
{
  _desiredX = zoomAt.x();
  _desiredY = zoomAt.y();
  zoom(pct);
}

/** Sets the current zoom percentage to the given value. */
void
ZImageView::zoom(float pct)
{
  _zoom = qBound(0.0f, pct, 1.0f);
  repaint();
}

/** Zooms into the image by 10% */
void
ZImageView::zoomIn()
{
  zoom(_zoom + .1);
}

/** Zooms away from the image by 10% */
void
ZImageView::zoomOut()
{
  zoom(_zoom - .1);
}

/** Responds to the user pressing a mouse button. */
void
ZImageView::mousePressEvent(QMouseEvent *e)
{
  e->accept();
  setCursor(CURSOR_MOUSE_PRESS);
  _mouseX = e->x();
  _mouseY = e->y();
}

/** Responds to the user releasing a mouse button. */
void
ZImageView::mouseReleaseEvent(QMouseEvent *e)
{
  e->accept();
  setCursor(CURSOR_NORMAL);
  updateViewport();
  resetZoomPoint();
}

/** Responds to the user double-clicking a mouse button on the image. A left
 * double-click zooms in on the image and a right double-click zooms out.
 * Zooming is centered on the location of the double-click. */
void
ZImageView::mouseDoubleClickEvent(QMouseEvent *e)
{
  e->accept();

  QPoint center = rect().center();
  int dx = e->x() - center.x();
  int dy = e->y() - center.y();
  updateViewport(dx, dy);
  resetZoomPoint();

  Qt::MouseButton btn = e->button();
  if (btn == Qt::LeftButton)
    zoomIn();
  else if (btn == Qt::RightButton)
    zoomOut();
}

/** Responds to the user moving the mouse. */
void
ZImageView::mouseMoveEvent(QMouseEvent *e)
{
  e->accept();
  int dx = _mouseX - e->x();
  int dy = _mouseY - e->y();
  _mouseX = e->x();
  _mouseY = e->y();

  updateViewport(dx, dy);
  if (0.001 <= _zoom) {
    repaint();
  }
}

void
ZImageView::wheelEvent(QWheelEvent *e)
{
  if (e->delta() > 0) {
    zoomIn();
  } else {
    zoomOut();
  }
}
