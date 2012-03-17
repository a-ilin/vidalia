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
** \file BridgeDownloaderProgressDialog.cpp
** \brief Displays the progress of a request for bridge addresses
*/

#include "BridgeDownloaderProgressDialog.h"

#include <QTimer>


BridgeDownloaderProgressDialog::BridgeDownloaderProgressDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton *)),
          this, SLOT(buttonClicked(QAbstractButton *)));

  setModal(true);
}

void
BridgeDownloaderProgressDialog::setVisible(bool visible)
{
  if (visible) {
    ui.progressBar->setRange(0, 0);
    ui.buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
  }
  QDialog::setVisible(visible);
}

void
BridgeDownloaderProgressDialog::setStatus(const QString &status)
{
  ui.lblStatus->setText(status);
}

void
BridgeDownloaderProgressDialog::setDownloadProgress(qint64 done, qint64 total)
{
  ui.progressBar->setRange(0, total);
  ui.progressBar->setValue(done);
}

void
BridgeDownloaderProgressDialog::bridgeRequestFinished(const QStringList &bridges)
{
  Q_UNUSED(bridges);

  accept();
}

void
BridgeDownloaderProgressDialog::bridgeRequestFailed(const QString &error)
{
  ui.lblStatus->setText(tr("Unable to download bridges: %1").arg(error));

  ui.progressBar->setRange(0, 1);
  ui.progressBar->setValue(1);

  ui.buttonBox->setStandardButtons(QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Retry
                                     | QDialogButtonBox::Help);
}

void
BridgeDownloaderProgressDialog::buttonClicked(QAbstractButton *button)
{
  int standardButton = ui.buttonBox->standardButton(button);
  if (standardButton == QDialogButtonBox::Retry) {
    setStatus(tr("Retrying bridge request..."));
    setDownloadProgress(0, 0);
    ui.buttonBox->setStandardButtons(QDialogButtonBox::Cancel);

    QTimer::singleShot(1000, this, SIGNAL(retry()));
  } else {
    done(standardButton);
  }
}

