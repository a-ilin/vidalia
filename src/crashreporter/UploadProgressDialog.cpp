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
** \file UploadProgressDialog.cpp
** \brief Displays the progress of uploading a crash report to the server
*/

#include "UploadProgressDialog.h"


UploadProgressDialog::UploadProgressDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);
 
  setModal(true);
}

void
UploadProgressDialog::setVisible(bool visible)
{
  if (visible) {
    ui.progressBar->setRange(0, 0);
    ui.buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
  }
  QDialog::setVisible(visible);
}

void
UploadProgressDialog::setStatus(const QString &status)
{
  ui.lblStatus->setText(status);
}

void
UploadProgressDialog::setUploadProgress(int done, int total)
{
  ui.progressBar->setRange(0, total);
  ui.progressBar->setValue(done);
}

void
UploadProgressDialog::uploadFailed(const QString &error)
{
  ui.lblStatus->setText(tr("Unable to send report: %1").arg(error));

  ui.progressBar->setRange(0, 1);
  ui.progressBar->setValue(1);

  ui.buttonBox->setStandardButtons(QDialogButtonBox::Ok);
}

