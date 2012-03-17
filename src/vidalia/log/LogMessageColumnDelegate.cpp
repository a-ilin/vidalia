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
** \file LogMessageColumnDelegate.cpp
** \brief Delegate responsible for rendering the log message column
*/

#include "LogMessageColumnDelegate.h"


/** Default constructor. */
LogMessageColumnDelegate::LogMessageColumnDelegate(QObject *parent)
  : QItemDelegate(parent)
{
}

/** Overrides the default paint() method so that we can prevent Qt from
 * munging Tor's log messages when using a Right-to-Left layout (e.g. when
 * viewing Vidalia in Farsi). */
void
LogMessageColumnDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
  QStyleOptionViewItem styleOption = option;
  styleOption.direction = Qt::LeftToRight;

  QItemDelegate::paint(painter, styleOption, index);
}

