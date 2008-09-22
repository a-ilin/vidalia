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
#include <QDialog>
#include <vidalia.h>

#include "aboutdialog.h"
#include "licensedialog.h"


/** Default Constructor. */
AboutDialog::AboutDialog(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint)
{
  ui.setupUi(this);

  /* Get Vidalia's version number */
  ui.lblVidaliaVersion->setText(Vidalia::version());

  /* Get Qt's version number */
  ui.lblQtVersion->setText(QT_VERSION_STR);

  /* Display the license information dialog when the "License" button 
   * is clicked. */
  connect(ui.btnShowLicense, SIGNAL(clicked()),
          new LicenseDialog(this), SLOT(exec()));
}

/** Displays the About dialog window **/
void
AboutDialog::setVisible(bool visible)
{
  if (visible) {
    /* Access the TorControl object to retrieve version */
    TorControl *tc = Vidalia::torControl();
    if (tc->isRunning()) {
      QString version = tc->getTorVersionString();
      if (version.isEmpty()) {
        version = tr("Unavailable");
      }
      ui.lblTorVersion->setText(version);
    } else {
      ui.lblTorVersion->setText(tr("Not Running"));
    }
  }
  adjustSize();
  QDialog::setVisible(visible);
}

