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
 * \file logheaderview.h
 * \version $Id$ 
 * \brief Header for the message log QTreeView
 */

#ifndef _LOGHEADERVIEW_H
#define _LOGHEADERVIEW_H

#include <QHeaderView>


class LogHeaderView : public QHeaderView
{
  Q_OBJECT

public:
  /** Default constructor. */
  LogHeaderView(QWidget *parent);

  /** Resets all column widths back to their defaults. */
  void resetColumnWidths();
  /** Resizes the column headers based on the longest message item. */
  void resize(int hint);
};

#endif

