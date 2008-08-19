/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file vclicklabel.cpp
** \version $Id$
** \brief Custom widget to create a clickable label with both an image and text.
*/

#include <QPainter>
#include <vidalia.h>

#include "vclicklabel.h"


/** Default constructor. */
VClickLabel::VClickLabel(QWidget *parent)
 : QWidget(parent)
{
  setCursor(Qt::PointingHandCursor);
}

/** Returns the current size hint for this widget's current contents. */
QSize
VClickLabel::sizeHint() const
{
  int height = qMax(_pixmap.height(), fontMetrics().height())+2;
  int width = _pixmap.width() + fontMetrics().width(_text)+2;
  return QSize(width, height);
}

/** Returns the minimum size hint for this widget's current contents. */
QSize
VClickLabel::minimumSizeHint() const
{
  return sizeHint();
}

/** Overloaded paint event to draw a pixmap and a text label. */
void
VClickLabel::paintEvent(QPaintEvent *e)
{
  QPainter p(this);
  QRect rect = this->rect();

  if (vApp->isLeftToRight()) {
    if (!_pixmap.isNull())
      p.drawPixmap(0, qMax((rect.height()-_pixmap.height())/2, 0), _pixmap);
    if (!_text.isEmpty())
      p.drawText(_pixmap.width()+2, (rect.height()+fontInfo().pixelSize())/2, _text);
  } else {
    if (!_pixmap.isNull())
      p.drawPixmap(qMax(rect.right()-_pixmap.width(), 0),
                   qMax((rect.height()-_pixmap.height())/2, 0), _pixmap);
    if (!_text.isEmpty()) {
      int textWidth  = fontMetrics().width(_text);
      p.drawText(qMax(rect.right()-_pixmap.width()-textWidth-2, 0),
                 (rect.height()+fontInfo().pixelSize())/2, _text);
    }
  }
  e->accept();
}

/** Overloaded mouse event to catch left mouse button clicks. */
void
VClickLabel::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    emit clicked();
  }
  e->accept();
}

/** Sets the label text to <b>text</b>. */
void
VClickLabel::setText(const QString &text)
{
  _text = text;
  update();
}

/** Sets the widget's image to <b>img</b>. */
void
VClickLabel::setPixmap(const QPixmap &pixmap)
{
  _pixmap = pixmap;
  update();
}

