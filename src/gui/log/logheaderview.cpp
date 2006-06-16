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
 * \file logheaderview.cpp
 * \version $Id: messagelog.cpp 797 2006-05-09 04:42:35Z edmanm $ 
 */

#include "logheaderview.h"
#include "logtreewidget.h"


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

#include <QtDebug>

/** Resizes the column headers based on the longest message item. */
void
LogHeaderView::resize(int hint)
{
  int size = sectionSize(COL_MESG);
  if (hint > size) {
    /* The message is wider than the window, so expand the column */
    setStretchLastSection(false);
    resizeSection(COL_MESG, hint);
  } else if (hint < size) {
    /* The message is short, so just stretch the last column to the end */
    setStretchLastSection(true);
  }
}

