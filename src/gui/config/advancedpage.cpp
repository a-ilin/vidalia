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
 * \file advancedpage.cpp
 * \version $Id$
 */
 
#include "advancedpage.h"

/** Constructor */
AdvancedPage::AdvancedPage(QWidget *parent)
: QWidget(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Bind event to actions */
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));

  /* Hide platform specific features */
#ifdef Q_WS_WIN
  ui.grpPermissions->setVisible(false);
#endif
}

/* Saves all settings for this page */
void
AdvancedPage::saveChanges(TorSettings* torSettings)
{
  torSettings->setControlPort(ui.lineControlPort->text().toUShort());
  torSettings->setTorrc(ui.lineTorConfig->text());
  torSettings->setUser(ui.lineUser->text());
  torSettings->setGroup(ui.lineGroup->text());
}

/* Loads previously saved settings */
void
AdvancedPage::loadSettings(TorSettings* torSettings)
{
  ui.lineControlPort->setText(QString::number(torSettings->getControlPort()));
  ui.lineTorConfig->setText(torSettings->getTorrc());
  ui.lineUser->setText(torSettings->getUser());
  ui.lineGroup->setText(torSettings->getGroup());
}

/** Open a QFileDialog to browse for Tor config file. */
void
AdvancedPage::browseTorConfig()
{
  QString filename = QDir::convertSeparators(
                       QFileDialog::getOpenFileName(this,
                         tr("Select Tor Configuration File")));
  if (!filename.isEmpty()) {
    ui.lineTorConfig->setText(filename);
  }
}

