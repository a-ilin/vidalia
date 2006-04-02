/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file aboutdialog.cpp
 * \version $Id$
 */

#include <vidalia.h>
#include "aboutdialog.h"

/** Default Constructor **/
AboutDialog::AboutDialog(QWidget *parent)
: QDialog(parent)
{
  ui.setupUi(this);

  /* Save the dialog's size limits */
  _minSize = minimumSize();
  _maxSize = maximumSize();

  /* Save the TorControl object to use later */
  _torControl = Vidalia::torControl();

  /* Get Vidalia's version number */
  ui.lblVidaliaVersion->setText(Vidalia::version());

  /* Get Qt's version number */
  ui.lblQtVersion->setText(QT_VERSION_STR);

  /* Load the brief licensing information and hide it initally */
  loadLicense();
  showLicense(false);
 
  /* Connect the few signals we'll need */
  connect(ui.btnLicense, SIGNAL(toggled(bool)),
          this, SLOT(showLicense(bool)));
  connect(ui.btnOK, SIGNAL(clicked()),
          this, SLOT(hide()));
}

/** Hides the licensing information and then hides the About dialog. */
void
AboutDialog::hide()
{
  showLicense(false);
  QWidget::hide();
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

/** Displays the licensing information */
void
AboutDialog::showLicense(bool show)
{
  ui.frmLicense->setVisible(show);
  if (show) {
    resize(_maxSize);
    ui.btnLicense->setChecked(true);
    ui.btnLicense->setText(tr("Hide License"));
  } else {
    resize(_minSize);
    ui.btnLicense->setChecked(false);
    ui.btnLicense->setText(tr("View License"));
  }
}

/** Displays the About dialog window **/
void
AboutDialog::show()
{
  /* Access the TorControl object to retrieve version */
  if (_torControl->isRunning()) {
    ui.lblTorVersion->setText(_torControl->getTorVersion());
  } else {
    ui.lblTorVersion->setText(tr("<Not Running>"));
  }

  if (!this->isVisible()) {
    QDialog::show();
  } else {
    QDialog::activateWindow();
    QDialog::raise();
  }
}

/** Resizes the dialog to the given size and sets the minimum and maximum size
 * of the dialog to the given size. */
void
AboutDialog::resize(QSize size)
{
  setMinimumSize(size);
  setMaximumSize(size);
  QWidget::resize(size);
}

