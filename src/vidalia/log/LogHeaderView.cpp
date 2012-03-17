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
** \file LogHeaderView.cpp
** \brief Header for the message log QTreeView
*/

#include "LogHeaderView.h"
#include "LogTreeWidget.h"

/* Column indices */
#define COL_TIME  LogTreeWidget::TimeColumn
#define COL_TYPE  LogTreeWidget::TypeColumn
#define COL_MESG  LogTreeWidget::MessageColumn

/* Default column widths */
#define COL_TIME_WIDTH    135
#define COL_TYPE_WIDTH    70


/** Default constructor. */
LogHeaderView::LogHeaderView(QWidget *parent)
  : QHeaderView(Qt::Horizontal, parent)
{
}

/** Resets all column widths back to their defaults. */
void
LogHeaderView::resetColumnWidths()
{
  resizeSection(COL_TIME, COL_TIME_WIDTH);
  resizeSection(COL_TYPE, COL_TYPE_WIDTH);
  setStretchLastSection(true);
}


