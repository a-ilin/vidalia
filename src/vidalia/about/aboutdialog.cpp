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
** \file aboutdialog.cpp
** \version $Id$
** \brief Displays information about Vidalia, Tor, and Qt
*/

#include <QFile>
#include <vidalia.h>
#include "aboutdialog.h"


/** Default Constructor **/
AboutDialog::AboutDialog(QWidget *parent, Qt::WFlags flags)
: VidaliaWindow("AboutDialog", parent, flags)
{
  ui.setupUi(this);

  /* Pressing 'Esc' or 'Ctrl+W' will close the window */
  setShortcut("Esc", SLOT(close()));
  setShortcut("Ctrl+W", SLOT(close()));

  /* Save the TorControl object to use later */
  _torControl = Vidalia::torControl();

  /* Get Vidalia's version number */
  ui.lblVidaliaVersion->setText(Vidalia::version());

  /* Get Qt's version number */
  ui.lblQtVersion->setText(QT_VERSION_STR);

  /* Load the brief licensing information and hide it initally */
  loadLicense();
}

/** Loads the license information */
void
AboutDialog::loadLicense()
{
  QFile licenseFile(":/docs/short_license.txt");
  licenseFile.open(QFile::ReadOnly);
  ui.txtLicense->setPlainText(licenseFile.readAll());
  licenseFile.close();
}

/** Displays the About dialog window **/
void
AboutDialog::showWindow()
{
  /* Access the TorControl object to retrieve version */
  if (_torControl->isRunning()) {
    QString version = _torControl->getTorVersionString();
    if (version.isEmpty()) {
      version = tr("<Unavailable>");
    }
    ui.lblTorVersion->setText(version);
  } else {
    ui.lblTorVersion->setText(tr("<Not Running>"));
  }
  VidaliaWindow::showWindow();
}

