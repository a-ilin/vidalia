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
** \file AboutDialog.cpp
** \brief Displays information about Vidalia, Tor, and Qt
*/

#include "AboutDialog.h"
#include "LicenseDialog.h"
#include "Vidalia.h"

#include <QFile>
#include <QDialog>
#include <QPushButton>


/** Default Constructor. */
AboutDialog::AboutDialog(QWidget *parent, Qt::WindowFlags flags)
  : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint)
{
  ui.setupUi(this);

  /* Add a "License" button to the button box at the bottom */
  QPushButton *licenseButton;
  licenseButton = ui.buttonBox->addButton(tr("License"),
                                          QDialogButtonBox::ActionRole);

  /* Get Vidalia's version number */
  ui.lblVidaliaVersion->setText(QString("Vidalia %1").arg(Vidalia::version()));

  /* Get Tor's version number or hide it if Tor isn't running */
  if (Vidalia::torControl()->isConnected()) {
    QString version = Vidalia::torControl()->getTorVersionString();
    if (! version.isEmpty())
      ui.lblTorVersion->setText(QString("Tor %1").arg(version));
    else
      ui.lblTorVersion->setVisible(false);
  } else {
    ui.lblTorVersion->setVisible(false);
  }

  /* Get Qt's version number */
  ui.lblQtVersion->setText(QString("Qt %1").arg(QT_VERSION_STR));

  /* Display the license information dialog when the "License" button
   * is clicked. */
  connect(licenseButton, SIGNAL(clicked()),
          new LicenseDialog(this), SLOT(exec()));

  /* Close this dialog when the "Close" button is clicked */
  connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

