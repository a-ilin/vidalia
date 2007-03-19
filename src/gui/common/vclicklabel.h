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
 * \file vclicklabel.h
 * \version $Id$
 * \brief Custom widget to create a clickable label with both an image and text.
 */

#ifndef _VCLICKLABEL_H
#define _VCLICKLABEL_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <QSize>


class VClickLabel : public QWidget
{
  Q_OBJECT

public:
  /** Default constructor. */
  VClickLabel(QWidget *parent = 0);

  /** Returns the current size hint for this widget's current contents. */
  virtual QSize sizeHint() const;
  /** Returns the minimum size hint for this widget's current contents. */
  virtual QSize minimumSizeHint() const;
  
  /** Sets the label text to <b>text</b>. */
  void setText(const QString &text);
  /** Sets the widget's image to <b>img</b>. */
  void setPixmap(const QPixmap &img);

signals:
  /** Emitted when the widget is left-clicked. */
  void clicked();
  
protected:
  /** Overloaded paint event to draw a pixmap and a text label. */
  virtual void paintEvent(QPaintEvent *e);
  /** Overloaded mouse event to catch left mouse button clicks. */
  virtual void mouseReleaseEvent(QMouseEvent *e);

private:
  QString _text;    /**< Text label to display in the widget. */
  QPixmap _pixmap;  /**< Image to display in the widget. */
};

#endif

