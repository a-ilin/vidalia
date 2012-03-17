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
** \file BridgeUsageDialog.cpp
** \brief Displays a summary of bridge usage information, including client
** geographic location history.
*/

#include "BridgeUsageDialog.h"
#include "CountryInfo.h"

#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QPixmap>


BridgeUsageDialog::BridgeUsageDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);
  ui.treeClientSummary->setHeaderLabels(QStringList() << QString("")
                                                      << tr("Country")
                                                      << tr("# Clients"));
}

void
BridgeUsageDialog::showEvent(QShowEvent *e)
{
  QHeaderView *header = ui.treeClientSummary->header();
  header->setResizeMode(0, QHeaderView::ResizeToContents);
  header->resizeSection(1, 220);
  header->setResizeMode(2, QHeaderView::ResizeToContents);

  QDialog::showEvent(e);
}

void
BridgeUsageDialog::update(const QDateTime &timeStarted,
                          const QHash<QString,int> &countrySummary)
{
  QTreeWidgetItem *item;
  int minClients, maxClients;
  QString countryName;
  QPixmap flag;

  /* Set the header with the TimeStarted value converted to local time */
  ui.lblClientSummary->setText(tr("Clients from the following countries have "
                                  "used your relay since %1")
                                  .arg(timeStarted.toLocalTime().toString()));

  /* Populate the table of client country statistics */
  foreach (QString countryCode, countrySummary.keys()) {
    maxClients = countrySummary.value(countryCode);
    minClients = maxClients-7;

    flag = QPixmap(":/images/flags/" + countryCode.toLower() + ".png");
    if (flag.isNull())
      flag = QPixmap(":/images/flags/unknown.png");

    countryName = CountryInfo::countryName(countryCode);
    if (countryName.isEmpty())
      countryName = countryCode;

    item = new QTreeWidgetItem();
    item->setIcon(0, QIcon(flag));
    item->setText(1, countryName);
    item->setText(2, QString("%1-%2").arg(minClients).arg(maxClients));
    ui.treeClientSummary->addTopLevelItem(item);
  }
  ui.treeClientSummary->sortItems(2, Qt::DescendingOrder);
}

