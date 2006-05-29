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
 * \file zimageview.cpp
 * \version $Id: netviewer.cpp 699 2006-04-15 03:12:22Z hipplej $
 */

#include "zimageview.h"

#include <cmath>

#include <QPainter>
#include <QMouseEvent>

#define MIN_SIZE QSize(512,256)

/** Constructor. */
ZImageView::ZImageView(QWidget *parent)
  : QWidget(parent)
{
  /* Initialize members */
  _zoom = 0.0;
  _mouseDown = false;
  _maxZoomFactor = 8.0;
  _padding = 60;

  setCursor(QCursor(Qt::CrossCursor));
  updateViewport();
  resetZoomPoint();
  repaint();
}

/** Ssets the displayed image. */
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

  QImage i;
  i = _image.copy(r).scaled(scaleTo,
		    Qt::KeepAspectRatioByExpanding,
		    Qt::SmoothTransformation);

  int extraWidth = int(double(sRect.width() - i.width()) / 2.0);
  int extraHeight = int(double(sRect.height() - i.height()) / 2.0);

  // Paint the white borders, then paint the image.
  // We don't want to paint the background
  // because this isn't double buffered and that would flicker.
  // We could double buffer it, but that would cost ~3 MB of memory.
  
  QPainter p(this);
  p.setPen(QPen(QColor("#cccccc"), 1));
  int x1 = extraWidth - 1;
  int y1 = extraHeight - 1;
  int x2 = extraWidth - 1 + i.width() + 2;
  int y2 = extraHeight - 1 + i.height() + 2;

  if (extraWidth > 0) {
    p.fillRect(0, 0, extraWidth, sRect.height(), background);
    p.fillRect(sRect.width() - extraWidth, 0,
	       sRect.width(), sRect.height(), background);

    p.drawLine(x1, y1, x1, y2);
    p.drawLine(x2, y1, x2, y2);
  }

  if (extraHeight > 0) {
    p.fillRect(0, 0, sRect.width(), extraHeight, background);
    p.fillRect(0, sRect.height() - extraHeight,
	       sRect.width(), sRect.height(), background);

    p.drawLine(x1,y1,x2,y1);
    p.drawLine(x1,y2,x2,y2);
  }

  // Finally, paint the image.
  p.drawImage(extraWidth, extraHeight, i);
}
	
/** Updates the displayed viewport. */
QPair<QRect, QRect>
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
  _desiredX = clamp(_desiredX + vdx, 0.0f, iw);
  _desiredY = clamp(_desiredY + vdy, 0.0f, ih);
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

  return QPair<QRect, QRect>(QRect(0, 0, int(newmaxw), int(newmaxh)),
			     QRect(0, 0, int(newminw), int(newminh)));
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

/** Sets the current zoom percentage to the given value. */
void
ZImageView::zoom(float pct)
{
  _zoom = clamp(pct, 0.0f, 1.0f);
  repaint();
}

/** Zooms into the image by 5% */
void
ZImageView::zoomIn()
{
  zoom(_zoom + .05);
}

/** Zooms away from the image by 5% */
void
ZImageView::zoomOut()
{
  zoom(_zoom - .05);
}

/** Responds to the user pressing a mouse button. */
void
ZImageView::mousePressEvent(QMouseEvent* e)
{
  e->accept();
  setCursor(QCursor(Qt::SizeAllCursor));
  _mouseDown = true;
  _mouseX = e->x();
  _mouseY = e->y();
  resetZoomPoint();
}

/** Responds to the user releasing a mouse button. */
void ZImageView::mouseReleaseEvent(QMouseEvent* e)
{
  e->accept();
  setCursor(QCursor(Qt::CrossCursor));
  updateViewport();
  resetZoomPoint();
  _mouseDown = false;
}

/** Responds to the user moving the mouse. */
void
ZImageView::mouseMoveEvent(QMouseEvent* e)
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

/** Handles the event indicating the widget has been resized. */
void
ZImageView::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
}

/* Untemplated version of clamp function in Zuerchertech ZUtil.h */
float
ZImageView::clamp(const float value, const float min, const float max)
{
  float trueMin = min < max ? min : max;
  float trueMax = min < max ? max : min;
  
  if (value < trueMin) {
    return trueMin;
  }
  if (trueMax < value) {
    return trueMax;
  }
  return value;
}

QSize
ZImageView::minimumSizeHint() const
{
  return MIN_SIZE;
}

