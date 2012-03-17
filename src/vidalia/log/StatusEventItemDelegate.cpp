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
** \file StatusEventItemDelegate.cpp
** \brief Handles custom painting of items in a StatusEventWidget
*/

#include "StatusEventItemDelegate.h"
#include "StatusEventItem.h"

#include "Vidalia.h"

#include <QPainter>
#include <QTextLine>
#include <QTextLayout>

StatusEventItemDelegate::StatusEventItemDelegate(QObject *parent)
  : QItemDelegate(parent)
{
  _helpIcon = QPixmap(":/images/16x16/system-help.png");
}

void
StatusEventItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
  QItemDelegate::paint(painter, option, index);

  painter->save();
  if (option.state & QStyle::State_Selected)
    painter->setPen(option.palette.highlightedText().color());

  QPixmap icon  = index.data(StatusEventItem::IconRole).value<QPixmap>();
  QTime tstamp  = index.data(StatusEventItem::TimestampRole).toTime();
  QString title = index.data(StatusEventItem::TitleRole).toString();
  QString text  = index.data(StatusEventItem::DescriptionRole).toString();
  QFont font    = option.font;
  QFontMetrics fm = option.fontMetrics;

  /* XXX: Handle right-to-left layouts here. */
  QRect iconRect(option.rect.x(),
                 option.rect.y(),
                 qMax(fm.width(tstamp.toString()), icon.width()) + 16,
                 option.rect.height());
  QRect textRect(iconRect.topRight(), option.rect.bottomRight());

  // Draw the status icon
  QPoint center = iconRect.center();
  int x = center.x() - qRound(icon.width() / 2.0);
  int y = center.y() - qRound((icon.height() + fm.lineSpacing()) / 2.0);
  painter->drawPixmap(x, y, icon);

  // Draw the timestamp text underneath the status icon
  x = iconRect.x();
  y = y + icon.height();
  painter->drawText(x, y,
                    iconRect.width(),
                    fm.lineSpacing(),
                    Qt::AlignCenter,
                    tstamp.toString());

  // Draw the event's title in a bold font. If the current item has an
  // associated help URL, draw the little "?" icon to the right of the
  // title text
  font.setBold(true);
  painter->setFont(font);
  if (! index.data(StatusEventItem::HelpUrlRole).isNull()) {
    // Draw the little "?" icon in the corner of the list item and
    // account for it when eliding the title
    title = fm.elidedText(title,
                          Qt::ElideRight,
                          textRect.width() - _helpIcon.width() - 24);

    x = textRect.topRight().x() - _helpIcon.width() - 8;
    y = textRect.y() + 8;
    painter->drawPixmap(x, y, _helpIcon);
  } else {
    title = fm.elidedText(title, Qt::ElideRight, textRect.width() - 16);
  }
  painter->drawText(textRect.x(),
                    textRect.y() + 8,
                    textRect.width(),
                    fm.lineSpacing(),
                    Qt::AlignVCenter | Qt::AlignLeft, title);

  // Draw the rest of the event text, up to a maximum of 2 lines for
  // unselected items or 5 lines for selected items. Any extra text will
  // be elided.
  font.setBold(false);
  painter->setFont(font);
  if (option.state & QStyle::State_Selected)
    text = layoutText(text, font, textRect.width(), 6).join("\n");
  else
    text = layoutText(text, font, textRect.width(), 3).join("\n");

  x = textRect.x();
  y = textRect.y() + 8 + fm.leading() + fm.lineSpacing();
  painter->drawText(x, y,
                    textRect.width(),
                    textRect.height() - (y - textRect.y()),
                    Qt::AlignTop | Qt::AlignLeft, text);

  painter->restore();
}

QSize
StatusEventItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
  int iconHeight, iconWidth;
  int textWidth, textHeight;
  QFontMetrics fontMetrics = option.fontMetrics;

  QPixmap icon = index.data(StatusEventItem::IconRole).value<QPixmap>();
  QString text = index.data(StatusEventItem::DescriptionRole).toString();
  QTime tstamp = index.data(StatusEventItem::TimestampRole).toTime();

  iconHeight = icon.height() + fontMetrics.lineSpacing() + 16;
  iconWidth  = qMax(fontMetrics.width(tstamp.toString()), icon.width()) + 16;
  textWidth  = option.rect.width() - iconWidth;

  if (option.state & QStyle::State_Selected)
    layoutText(text, option.font, textWidth, 6, &textHeight);
  else
    layoutText(text, option.font, textWidth, 3, &textHeight);
  textHeight += 8 + fontMetrics.leading() + fontMetrics.lineSpacing();

  return QSize(option.rect.width(), qMax(iconHeight, textHeight));
}

QStringList
StatusEventItemDelegate::layoutText(const QString &text,
                                    const QFont &font,
                                    int maxLineWidth,
                                    int maxLines,
                                    int *textHeight)
{
  QTextLayout textLayout(text, font);
  QFontMetrics fontMetrics(font);
  QStringList lines;
  qreal height = 0.0;

  textLayout.beginLayout();
  while (lines.size() < maxLines) {
    QTextLine line = textLayout.createLine();
    if (! line.isValid())
      break;
    if (maxLines <= 0 || lines.size() < maxLines-1) {
      // Wrap the current line at or below the maximum line width
      line.setLineWidth(maxLineWidth);
      lines.append(text.mid(line.textStart(), line.textLength()));
    } else {
      // Set the line width beyond the max line width, and then elide it
      // so the user has a visible indication that the full message is
      // longer than what is visible.
      line.setLineWidth(2 * maxLineWidth);
      lines.append(fontMetrics.elidedText(text.mid(line.textStart()),
                                          Qt::ElideRight,
                                          maxLineWidth));
    }
    height += fontMetrics.leading() + line.height();
  }
  textLayout.endLayout();

  if (textHeight)
    *textHeight = qRound(height);

  return lines;
}

