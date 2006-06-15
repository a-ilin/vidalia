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
 * \file logtreewidget.h
 * \version $Id: messagelog.cpp 797 2006-05-09 04:42:35Z edmanm $ 
 */

#ifndef _LOGTREEWIDGET_H
#define _LOGTREEWIDGET_H

#include <QList>
#include <QString>
#include <QTreeWidget>
#include <QHeaderView>
#include <QShowEvent>
#include <control/logevent.h>

#include "logtreeitem.h"


class LogTreeWidget : public QTreeWidget
{
  Q_OBJECT

public:

  /** Log tree column indices. */
  enum LogColumns {
    TimeColumn    = 0, /**< Timestamp column. */
    TypeColumn    = 1, /**< Message severity type column. */
    MessageColumn = 2  /**< Message text column. */
  };
  
  /** Default constructor. */
  LogTreeWidget(QWidget *parent = 0);

  /** Adds a message log item. */
  void addItem(LogTreeItem *item);
  /** Returns a list of all currently selected items. */
  QList<LogTreeItem *> selectedItems();
  /** Returns a list of all selected items as a formatted string. */
  QString selectedItemsText();
  /** Returns a list of all items in the tree. */
  QList<LogTreeItem *> allItems();
  /** Deselects all currently selected items. */
  void deselectAll();
  
  /** Returns the number of items currently in the tree. */
  int itemCount();
  /** Sets the maximum number of items in the tree. */
  void setMaximumItemCount(int max);
  /** Filters the log according to the specified filter. */
  void filter(uint filter);
  
  /** Adds a log item to the tree. */
  LogTreeItem* log(LogEvent::Severity type, QString message);
  
  /** Searches the log for entries that contain the given text. */
  QList<LogTreeItem *> find(QString text, bool highlight = true);

  /** Adjusts the message column, for long messages. */
  void adjustMessageColumn();

public slots:
  /** Clears all contents on the message log and resets the counter. */
  void clear();

protected:
  /** Sets the default, initial column header widths. */
  void showEvent(QShowEvent *event);

private slots:
  /** Called when the user moves the vertical scroll bar. */
  void onVerticalScroll(int value);

private:
  /** Casts a QList of one pointer type to another. */
  QList<LogTreeItem *> qlist_cast(QList<QTreeWidgetItem *> inlist);
  /** Sortrs a QList of pointers to tree items. */
  QList<LogTreeItem *> qlist_sort(QList<LogTreeItem *> inlist);

  int _maxItemCount; /**< Maximum number of items in the tree. */
  bool _scrollOnNewItem; /**< Set to true if we are to scroll to the new item
                               after adding a message to the log. */
};
  
#endif

