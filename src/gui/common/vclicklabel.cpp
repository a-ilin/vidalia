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
 * \file vclicklabel.cpp
 * \version $Id$
 * \brief Custom widget to create a clickable label with both an image and text.
 */

#include <QPainter>

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
  if (!_pixmap.isNull())
    p.drawPixmap(0, qMax((rect.height()-_pixmap.height())/2,0), _pixmap);
  if (!_text.isEmpty())
    p.drawText(_pixmap.width()+2, (rect.height()+fontInfo().pixelSize())/2, _text);
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

