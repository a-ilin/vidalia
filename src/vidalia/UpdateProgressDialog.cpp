/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#include "UpdateProgressDialog.h"


UpdateProgressDialog::UpdateProgressDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  connect(ui.btnHide, SIGNAL(clicked()), this, SLOT(onHide()));
  connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

  setModal(true);
}

void
UpdateProgressDialog::setStatus(UpdateProgressDialog::Status status)
{
  switch (status) {
    case CheckingForUpdates:
      ui.lblCurrentAction->setText(tr("Checking for available updates..."));

      ui.progressBar->setMinimum(0);
      ui.progressBar->setMaximum(0);

      ui.btnHide->setText(tr("Hide"));
      ui.btnCancel->setVisible(true);
      ui.btnCancel->setEnabled(true);
      break;

    case DownloadingUpdates:
      ui.lblCurrentAction->setText(tr("Downloading updates..."));
      break;

    case InstallingUpdates:
      ui.lblCurrentAction->setText(tr("Installing updated software..."));

      ui.progressBar->setMinimum(0);
      ui.progressBar->setMaximum(0);

      ui.btnCancel->setEnabled(false);
      break;

    case UpdatesInstalled:
      ui.lblCurrentAction->setText(tr("Done! Your software is now up to date."));

      ui.progressBar->setMinimum(0);
      ui.progressBar->setMaximum(1);
      ui.progressBar->setValue(1);

      ui.btnHide->setText(tr("OK"));
      ui.btnCancel->setVisible(false);
      break;

    default:
      break;
  }
}

void
UpdateProgressDialog::setDownloadProgress(const QString &url,
                                          int bytesReceived, int bytesTotal)
{
  Q_UNUSED(url);

  setStatus(DownloadingUpdates);
  ui.progressBar->setMaximum(bytesTotal);
  ui.progressBar->setValue(bytesReceived);
}

void
UpdateProgressDialog::onHide()
{
  setVisible(false);
}

void
UpdateProgressDialog::onCancel()
{
  emit cancelUpdate();
  hide();
}

