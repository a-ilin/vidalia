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
** \file StatusEventItem.cpp
** \version $Id$
** \brief Represents a single status event item in a StatusEventWidget
*/

#include "StatusEventItem.h"

#include <QTime>
#include <QPixmap>
#include <QString>

StatusEventItem::StatusEventItem(QTreeWidget *parent)
  : QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
}

void
StatusEventItem::setTimestamp(const QTime &timestamp)
{
  setData(0, TimestampRole, timestamp);
}

QTime
StatusEventItem::timestamp() const
{
  return data(0, TimestampRole).toTime();
}

void
StatusEventItem::setIcon(const QPixmap &pixmap)
{
  setData(0, IconRole, pixmap);
}

QPixmap
StatusEventItem::icon() const
{
  return data(0, IconRole).value<QPixmap>();
}

void
StatusEventItem::setTitle(const QString &title)
{
  setData(0, TitleRole, title);
}

QString
StatusEventItem::title() const
{
  return data(0, TitleRole).toString();
}

void
StatusEventItem::setDescription(const QString &description)
{
  setData(0, DescriptionRole, description);
}

QString
StatusEventItem::description() const
{
  return data(0, DescriptionRole).toString();
}

void
StatusEventItem::setHelpUrl(const QString &url)
{
  setData(0, HelpUrlRole, url);
}

QString
StatusEventItem::helpUrl() const
{
  return data(0, HelpUrlRole).toString();
}

void
StatusEventItem::setToolTip(const QString &toolTip)
{
  QTreeWidgetItem::setToolTip(0, toolTip);
}

bool
StatusEventItem::operator<(const QTreeWidgetItem &other) const
{
  QTime a = data(0, TimestampRole).toTime();
  QTime b = other.data(0, TimestampRole).toTime();

  return (a < b);
}

