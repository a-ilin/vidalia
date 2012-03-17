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
** \file UpdatesAvailableDialog.h
** \brief Displays a list of available updates and details, such as release
** notes. The user can choose to either install the updates now or later, or
** skip the updates entirely.
*/

#ifndef _UpdatesAvailableDialog_H
#define _UpdatesAvailableDialog_H

#include "ui_UpdatesAvailableDialog.h"
#include "PackageInfo.h"

#include <QDialog>
#include <QShowEvent>
#include <QTreeWidgetItem>


class UpdatesAvailableDialog : public QDialog
{
  Q_OBJECT

public:
  enum UpdatesAvailableDialogExitCode {
    InstallUpdatesNow   = 100,
    InstallUpdatesLater = 101,
  };

  /** Constructor. */
  UpdatesAvailableDialog(const PackageList &packageList, QWidget *parent = 0);

protected:
  /** Called when the dialog receives a QShowEvent. This simply adjusts
   * the column widths to something close to sane and forwards the event
   * to the parent.
   */
  virtual void showEvent(QShowEvent *e);

private slots:
  /** Called when the user selects a different package in the list. The widget
   * displaying details on the selected package will be updated.
   */
  void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

  /** Called when the user opts to install the displayed software updates
   * immediately.
   */
  void installUpdatesNow();

  /** Called when the user opts to install the display software updates at
   * a later time.
   */
  void installUpdatesLater();

private:
  /** Populates the table of available updates with package information
   * from <b>packageList</b>.
   */
  void loadPackagesTable(const PackageList &packageList);

  Ui::UpdatesAvailableDialog ui; /**< Qt Designer generated object. */
};

#endif

