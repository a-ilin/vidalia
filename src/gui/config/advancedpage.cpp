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

#include <QFile>
#include <QFileInfo>
#include <gui/common/vmessagebox.h>
#include "advancedpage.h"


/** Constructor */
AdvancedPage::AdvancedPage(QWidget *parent)
: ConfigPage(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Create TorSettings object */
  _settings = new TorSettings();

  /* Bind event to actions */
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));

  /* Hide platform specific features */
#ifdef Q_WS_WIN
  ui.grpPermissions->setVisible(false);
#endif
}

/** Destructor */
AdvancedPage::~AdvancedPage()
{
  delete _settings;
}

/* Saves all settings for this page */
bool
AdvancedPage::save(QString &errmsg)
{
  Q_UNUSED(errmsg);
  _settings->setControlPort(ui.lineControlPort->text().toUShort());
  _settings->setTorrc(ui.lineTorConfig->text());
  _settings->setUser(ui.lineUser->text());
  _settings->setGroup(ui.lineGroup->text());
  return true;
}

/* Loads previously saved settings */
void
AdvancedPage::load()
{
  ui.lineControlPort->setText(QString::number(_settings->getControlPort()));
  ui.lineTorConfig->setText(_settings->getTorrc());
  ui.lineUser->setText(_settings->getUser());
  ui.lineGroup->setText(_settings->getGroup());
}

/** Open a QFileDialog to browse for Tor config file. */
void
AdvancedPage::browseTorConfig()
{
  /* Create a new input dialog, which allows users to create files, too */
  QFileDialog *dialog = new QFileDialog(this, tr("Select Tor Configuration File"));
  dialog->setDirectory(QFileInfo(ui.lineTorConfig->text()).absoluteDir());
  dialog->selectFile(QFileInfo(ui.lineTorConfig->text()).fileName());
  dialog->setFileMode(QFileDialog::AnyFile);
  dialog->setReadOnly(false);

  /* Prompt the user to select a file or create a new one */
  if (!dialog->exec() || dialog->selectedFiles().isEmpty()) {
    return;
  }
  QString filename = QDir::convertSeparators(dialog->selectedFiles().at(0));
 
  /* Check if the file exists */
  QFile torrcFile(filename);
  if (!QFileInfo(filename).exists()) {
    /* The given file does not exist. Should we create it? */
    int response = VMessageBox::question(this,
                     tr("File Not Found"),
                     tr("%1 does not exist. Would you like to create it?")
                                                            .arg(filename),
                     VMessageBox::Yes, VMessageBox::No);
    
    if (response == VMessageBox::No) {
      /* Don't create it. Just bail. */
      return;
    }
    /* Attempt to create the specified file */
    if (!torrcFile.open(QIODevice::WriteOnly)) {
      VMessageBox::warning(this,
        tr("Failed to Create File"),
        tr("Unable to create %1 [%2]").arg(filename)
                                      .arg(torrcFile.errorString()),
        VMessageBox::Ok);
      return;
    }
  }
  ui.lineTorConfig->setText(filename);
}

