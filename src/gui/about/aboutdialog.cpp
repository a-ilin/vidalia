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
AboutDialog::AboutDialog(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  ui.setupUi(this);

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
AboutDialog::show()
{
  /* Access the TorControl object to retrieve version */
  if (_torControl->isRunning()) {
    ui.lblTorVersion->setText(_torControl->getTorVersion());
  } else {
    ui.lblTorVersion->setText(tr("<Not Running>"));
  }

  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    QMainWindow::activateWindow();
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    QMainWindow::raise();
  }
}
