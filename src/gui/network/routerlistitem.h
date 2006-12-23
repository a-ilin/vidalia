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
 * \file routerlistitem.h
 * \version $Id$
 * \brief Item representing a single router and status in a RouterListWidget
 */

#ifndef _ROUTERLISTITEM_H
#define _ROUTERLISTITEM_H

#include <QCoreApplication>
#include <QTreeWidgetItem>
#include <QString>
#include <control/routerdescriptor.h>
#include <util/geoip/geoip.h>

#include "routerlistwidget.h"

class RouterListWidget;


class RouterListItem : public QTreeWidgetItem
{
  Q_DECLARE_TR_FUNCTIONS(RouterListItem)

public:
  /** Default constructor. */
  RouterListItem(RouterListWidget *list, RouterDescriptor rd);
  /** Destructor. */
  ~RouterListItem();

  /** Updates this router item using a new descriptor. */
  void update(RouterDescriptor rd);
  /** Returns the router's ID. */
  QString id() const { return _rd->id(); }
  /** Returns the router's name. */
  QString name() const { return _rd->name(); }
  /** Returns the descriptor for this router. */
  RouterDescriptor descriptor() const { return *_rd; }
  /** Sets the location information for this router item. */
  void setLocation(GeoIp geoip);

  /** Overload the comparison operator. */
  virtual bool operator<(const QTreeWidgetItem &other) const;

private:
  RouterDescriptor* _rd;   /**< Descriptor for this router item. */
  RouterListWidget* _list; /**< The list for this list item. */
  qint64 _statusValue;     /**< Value used to sort items by status. */
  QString _country;        /**< Country in which this router is likely
                                located. */
};

#endif

