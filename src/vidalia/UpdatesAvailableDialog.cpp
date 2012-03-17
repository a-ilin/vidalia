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
** \file UpdatesAvailableDialog.cpp
** \brief Displays a list of available updates and details, such as release
** notes. The user can choose to either install the updates now or later, or
** skip the updates entirely.
*/

#include "UpdatesAvailableDialog.h"
#include "Vidalia.h"

#include <QHeaderView>


UpdatesAvailableDialog::UpdatesAvailableDialog(const PackageList &packageList,
                                               QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  connect(ui.btnInstall, SIGNAL(clicked()),
          this, SLOT(installUpdatesNow()));
  connect(ui.btnInstallLater, SIGNAL(clicked()),
          this, SLOT(installUpdatesLater()));

  connect(ui.treeUpdates,
          SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
          this, SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

  loadPackagesTable(packageList);
}

void
UpdatesAvailableDialog::showEvent(QShowEvent *e)
{
  ui.treeUpdates->header()->resizeSection(0, 240);
  ui.treeUpdates->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  QDialog::showEvent(e);
}

void
UpdatesAvailableDialog::loadPackagesTable(const PackageList &packageList)
{
  int row = 0;
  QString language;
  QTreeWidgetItem *item;

  language = Vidalia::language();

  foreach (PackageInfo package, packageList) {
    item = new QTreeWidgetItem(ui.treeUpdates);

    if (package.hasShortDescription(language))
      item->setText(0, package.shortDescription(language));
    else
      item->setText(0, package.shortDescription("en"));

    if (package.hasLongDescription(language))
      item->setData(0, Qt::UserRole, package.longDescription(language));
    else
      item->setData(0, Qt::UserRole, package.longDescription("en"));

    item->setText(1, package.version());
    ui.treeUpdates->insertTopLevelItem(row++, item);
  }
}

void
UpdatesAvailableDialog::currentItemChanged(QTreeWidgetItem *current,
                                         QTreeWidgetItem *previous)
{
  Q_UNUSED(previous);

  ui.textDetails->clear();
  if (current)
    ui.textDetails->setText(current->data(0, Qt::UserRole).toString());
}

void
UpdatesAvailableDialog::installUpdatesNow()
{
  done(InstallUpdatesNow);
}

void
UpdatesAvailableDialog::installUpdatesLater()
{
  done(InstallUpdatesLater);
}

