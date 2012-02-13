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
** \file RouterListItem.cpp
** \brief Item representing a single router and status in a RouterListWidget
*/

#include "RouterListItem.h"
#include "RouterListWidget.h"

#include <QHeaderView>

#define STATUS_COLUMN   (RouterListWidget::StatusColumn)
#define COUNTRY_COLUMN  (RouterListWidget::CountryColumn)
#define NAME_COLUMN     (RouterListWidget::NameColumn)

#define IMG_NODE_OFFLINE    ":/images/icons/node-unresponsive.png"
#define IMG_NODE_SLEEPING   ":/images/icons/node-hibernating.png"
#define IMG_NODE_NO_BW      ":/images/icons/node-bw-none.png"
#define IMG_NODE_LOW_BW     ":/images/icons/node-bw-low.png"
#define IMG_NODE_MED_BW     ":/images/icons/node-bw-med.png"
#define IMG_NODE_HIGH_BW    ":/images/icons/node-bw-high.png"
#define IMG_FLAG_UNKNOWN    ":/images/flags/unknown.png"


/** Default constructor. */
RouterListItem::RouterListItem(RouterListWidget *list, RouterDescriptor rd)
  : QTreeWidgetItem()
{
  _list = list;
  _rd   = 0;
  _countryCode = "~"; /* Force items with no country to the bottom */
  setIcon(COUNTRY_COLUMN, QIcon(IMG_FLAG_UNKNOWN));
  update(rd);
}

/** Destructor. */
RouterListItem::~RouterListItem()
{
  if (_rd)
    delete _rd;
}

/** Updates the router descriptor for this item. */
void
RouterListItem::update(const RouterDescriptor &rd)
{
  QIcon statusIcon;
  if (_rd) {
    delete _rd;
  }
  _rd = new RouterDescriptor(rd);
  
  /* Determine the status value (used for sorting) and icon */
  if (_rd->offline()) {
    _statusValue = -1;
    statusIcon = QIcon(IMG_NODE_OFFLINE);
    setToolTip(STATUS_COLUMN, tr("Offline"));
  } else if (_rd->hibernating()) {
    _statusValue = 0;
    statusIcon = QIcon(IMG_NODE_SLEEPING);
    setToolTip(STATUS_COLUMN, tr("Hibernating"));
  } else {
    _statusValue = (qint64)qMin(_rd->observedBandwidth(), 
                                qMin(_rd->averageBandwidth(),
                                     _rd->burstBandwidth()));
    if (_statusValue >= 400*1024) {
      statusIcon = QIcon(IMG_NODE_HIGH_BW);
    } else if (_statusValue >= 60*1024) {
      statusIcon = QIcon(IMG_NODE_MED_BW);
    } else if (_statusValue >= 20*1024) {
      statusIcon = QIcon(IMG_NODE_LOW_BW);
    } else {
      statusIcon = QIcon(IMG_NODE_NO_BW);
    }
    setToolTip(STATUS_COLUMN, tr("%1 KB/s").arg(_statusValue/1024));
  }
  
  /* Make the new information visible */
  setIcon(STATUS_COLUMN, statusIcon);
  setText(NAME_COLUMN, _rd->name());
  setToolTip(NAME_COLUMN, QString(_rd->name() + "\r\n" + _rd->platform()));
}

/** Sets the location information for this item's router descriptor. */
void
RouterListItem::setLocation(const GeoIpRecord &geoip)
{
  QPixmap flag(":/images/flags/" + geoip.countryCode().toLower() + ".png");
  if (!flag.isNull()) {
    setIcon(COUNTRY_COLUMN, QIcon(flag));
  }
  setToolTip(COUNTRY_COLUMN, geoip.toString());

  if (_rd)
    _rd->setLocation(geoip.toString());

  _location = geoip;
  _countryCode = geoip.countryCode();
}

/** Overload the comparison operator. */
bool
RouterListItem::operator<(const QTreeWidgetItem &other) const
{
  const RouterListItem *a = this;
  const RouterListItem *b = (RouterListItem *)&other;
 
  if (_list) {
    Qt::SortOrder order = _list->header()->sortIndicatorOrder();
    switch (_list->sortColumn()) {
      case RouterListWidget::StatusColumn:
        /* Numeric comparison based on status and/or bandwidth */
        if (a->_statusValue == b->_statusValue) {
          if (order == Qt::AscendingOrder)
            return (a->name().toLower() > b->name().toLower());
          else
            return (a->name().toLower() < b->name().toLower());
        }
        return (a->_statusValue < b->_statusValue);
      case RouterListWidget::CountryColumn:
        /* Compare based on country code */
        if (a->_countryCode == b->_countryCode) {
          if (order == Qt::AscendingOrder)
            return (a->_statusValue > b->_statusValue);
          else
            return (a->_statusValue < b->_statusValue);
        }
        return (a->_countryCode < b->_countryCode);
      case RouterListWidget::NameColumn:
        /* Case-insensitive comparison based on router name */
        if (a->name().toLower() == b->name().toLower()) {
          if (order == Qt::AscendingOrder)
            return (a->_statusValue > b->_statusValue);
          else
            return (a->_statusValue < b->_statusValue);
        }
        return (a->name().toLower() < b->name().toLower());
      default:
        break;
    }
  }
  return QTreeWidgetItem::operator<(other);
}

