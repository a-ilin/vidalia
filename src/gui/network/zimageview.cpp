#include "zimageview.h"

#include <cmath>

#include <QPainter>
#include <QMouseEvent>

#define MIN_SIZE QSize(512,256)

//=============================================================================
ZImageView::ZImageView(QWidget *parent)
  : QWidget(parent)
{
  /* Initialize members */
  _zoom = 0.0;
  _mouseDown = false;
  _maxZoomFactor = 8.0;
  _padding = 60;
  _paintRequested = false;

  setCursor(QCursor(Qt::CrossCursor));
  updateViewport();
  resetZoomPoint();
  paintNow();
  
  connect(&_timer, SIGNAL(timeout()), this, SLOT(polishNow()));
}

//=============================================================================
ZImageView::~ZImageView()
{}

//=============================================================================
void
ZImageView::setImage(QImage& img)
{
  _image = img.copy();
  updateViewport();
  resetZoomPoint();

  if (isVisible()) {
    polishNow();
  }
}

//=============================================================================
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
  i = _image.scaled(scaleTo,
		    Qt::IgnoreAspectRatio,
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

//=============================================================================
void
ZImageView::paintNow()
{
  // If the timer is not active, start it.
  // In a short time, it will go off and try to polish.  Any paints between now
  // and then will stop it from trying to polish until a later timeout.
  if (!_timer.isActive()) {
    _timer.start(90);
  } else {
    _paintRequested = true;
  }
  
  drawScaledImage();
}

//=============================================================================
void
ZImageView::polishNow()
{
  // If a paint has been recently requested, reset the flag to false, and wait.
  // If not, then the user has paused for a bit, so we have time to polish.

  if (_paintRequested) {
    _paintRequested = false;
  } else {
    drawScaledImage();
    _timer.stop();
  }
}
	
//=============================================================================
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

//=============================================================================
void
ZImageView::resetZoomPoint()
{
  QPoint viewCenter = _view.center();
  _desiredX = viewCenter.x();
  _desiredY = viewCenter.y();
}

//=============================================================================
void
ZImageView::paintEvent(QPaintEvent*)
{
  updateViewport();
  paintNow();
}

//=============================================================================
void
ZImageView::zoomInt(int pct)
{
  zoom(float(pct)/100.0);
}

//=============================================================================
void
ZImageView::zoom(float pct)
{
  _zoom = clamp(pct, 0.0f, 1.0f);
  updateViewport();
  paintNow();
}

//=============================================================================
float
ZImageView::getZoom()
{
  return _zoom;
}

//=============================================================================
int
ZImageView::getZoomInt()
{
  return int(100.0 * _zoom);
}

//=============================================================================
void
ZImageView::zoomWidth()
{
  // Get the rectangles for the max and min zoom values:
  QPair<QRect, QRect> bounds = updateViewport();

  QRect iRect = _image.rect();
  if (iRect.width() < iRect.height()) {
    // The image is taller than it is wide.
    // Therefore, we must zoom until it fills the width.
    float max = bounds.first.width();
    float min = bounds.second.width();
    float i = iRect.width();
    float rp = max - min + _padding;

    // updateViewport() finds the viewport size with (1-_zoom)rp + min = v
    // We want the scaled viewport to be equal to the image size + padding,
    // so set v = i + _padding and solve for _zoom.
    _zoom = 1.0 - (i + _padding - min) / rp;
    _zoom = clamp(_zoom, 0.0f, 1.0f);
    _desiredY = 0.0;
    updateViewport();
  }
  paintNow();
}

//=============================================================================
void
ZImageView::zoomHeight()
{
  // Get the rectangles for the max and min zoom values:
  QPair<QRect, QRect> bounds = updateViewport();

  QRect iRect = _image.rect();
  if (iRect.height() < iRect.width()) {
    // The image is wider than it is tall.  Therefore, we must zoom until it
    // fills the height.
    float max = bounds.first.height();
    float min = bounds.second.height();
    float i = iRect.height();
    float rp = max - min + _padding;

    // updateViewport() finds the viewport size with (1-_zoom)rp + min = v
    // We want the scaled viewport to be equal to the image size + padding,
    // so set v = i + _padding and solve for _zoom.
    _zoom = 1.0 - (i + _padding - min) / rp;
    _zoom = clamp(_zoom, 0.0f, 1.0f);
    _desiredX = 0.0;
    updateViewport();
  }
  paintNow();
}

//=============================================================================
void
ZImageView::mousePressEvent(QMouseEvent* e)
{
  e->accept();
  _mouseDown = true;
  _mouseX = e->x();
  _mouseY = e->y();
  resetZoomPoint();
}

//=============================================================================
void ZImageView::mouseReleaseEvent(QMouseEvent* e)
{
  e->accept();
  updateViewport();
  resetZoomPoint();
  _mouseDown = false;
}

//=============================================================================
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
    paintNow();
  }
}

//=============================================================================
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
