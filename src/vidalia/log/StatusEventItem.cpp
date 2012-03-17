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
** \file StatusEventItem.cpp
** \brief Represents a single status event item in a StatusEventWidget
*/

#include "StatusEventItem.h"

#include <QDateTime>
#include <QPixmap>
#include <QString>

StatusEventItem::StatusEventItem(QTreeWidget *parent)
  : QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
}

void
StatusEventItem::setTimestamp(const QDateTime &timestamp)
{
  setData(0, TimestampRole, timestamp);
}

QDateTime
StatusEventItem::timestamp() const
{
  return data(0, TimestampRole).toDateTime();
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

QString
StatusEventItem::toString() const
{
  return QString("[%1] %2 - %3").arg(timestamp().toString())
                                .arg(title())
                                .arg(description());
}

bool
StatusEventItem::operator<(const QTreeWidgetItem &other) const
{
  QDateTime a = data(0, TimestampRole).toDateTime();
  QDateTime b = other.data(0, TimestampRole).toDateTime();

  return (a < b);
}

