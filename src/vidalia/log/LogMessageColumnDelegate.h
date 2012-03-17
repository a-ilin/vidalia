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
** \file LogMessageColumnDelegate.h
** \brief Delegate responsible for rendering the log message column
*/

#ifndef _LOGMESSAGECOLUMNDELEGATE_H
#define _LOGMESSAGECOLUMNDELEGATE_H

#include <QItemDelegate>


class LogMessageColumnDelegate : public QItemDelegate
{
public:
  /** Default constructor. */
  LogMessageColumnDelegate(QObject *parent = 0);

  /** Overrides the default paint() method so that we can prevent Qt from
   * munging Tor's log messages when using a Right-to-Left layout (e.g. when
   * viewing Vidalia in Farsi). */
  virtual void paint(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;
};

#endif

