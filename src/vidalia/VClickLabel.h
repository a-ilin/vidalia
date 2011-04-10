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
** \file VClickLabel.h
** \brief Custom widget to create a clickable label with both an image and text.
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

