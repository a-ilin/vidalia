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
** \file StatusEventItemDelegate.h
** \brief Handles custom painting of items in a StatusEventWidget
*/

#ifndef _STATUSEVENTITEMDELEGATE_H
#define _STATUSEVENTITEMDELEGATE_H

#include <QItemDelegate>
#include <QPixmap>

class QStringList;

class StatusEventItemDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  StatusEventItemDelegate(QObject *parent = 0);

  /** Paints a status event item for <b>index</b> using the given paint
   * device <b>painter</b> and current style <b>option</b>s.
   */
  virtual void paint(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;

  /** Returns a QSize representing the desired dimensions needed to paint
   * a status event item for <b>index</b> using the specified style
   * <b>option</b>s.
   */
  virtual QSize sizeHint(const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

protected:
  /** Splits <b>text</b> at <b>maxLineWidth</b> pixels computed using the
   * font dimensions given by <b>fontMetrics</b> and returns a QStringList
   * containing at most <b>maxLines</b> of text. If the number of wrapped
   * lines is greater than or equal to <b>maxLines</b>, the last line of
   * text will be elided. If <b>maxLines</b> is less than or equal to 0, no
   * eliding will be done. If <b>textHeight</b> is not NULL, it will be set
   * to the height (in pixels) required to display the returned wrapped
   * lines.
   */
  static QStringList layoutText(const QString &text,
                                const QFont &fontMetrics,
                                int maxLineWidth,
                                int maxLines = -1,
                                int *textHeight = 0);

private:
  /** Small icon image drawn in the upper-right (or upper-left in RTL
   * layouts) for status events that have associated help URLs. */
  QPixmap _helpIcon;
};

#endif

