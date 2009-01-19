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
** \file routerlistitem.h
** \version $Id$
** \brief Item representing a single router and status in a RouterListWidget
*/

#ifndef _ROUTERLISTITEM_H
#define _ROUTERLISTITEM_H

#include <QCoreApplication>
#include <QTreeWidgetItem>
#include <QString>
#include <routerdescriptor.h>

#include "geoip.h"
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
  void update(const RouterDescriptor &rd);
  /** Returns the router's ID. */
  QString id() const { return _rd->id(); }
  /** Returns the router's name. */
  QString name() const { return _rd->name(); }
  /** Returns the descriptor for this router. */
  RouterDescriptor descriptor() const { return *_rd; }
  /** Sets the location information for this router item. */
  void setLocation(const GeoIp &geoip);
  /** Returns the location information set for this router item. */
  QString location() const { return _rd->location(); }

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

