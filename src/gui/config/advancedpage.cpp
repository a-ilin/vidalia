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
 * \brief Advanced Tor and Vidalia configuration options
 */

#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <gui/common/vmessagebox.h>
#include <util/file.h>
#include <vidalia.h>

#include "ipvalidator.h"
#include "advancedpage.h"

#if defined(Q_WS_WIN)
#include <control/torservice.h>
#endif


/** Constructor */
AdvancedPage::AdvancedPage(QWidget *parent)
: ConfigPage(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Create TorSettings object */
  _settings = new TorSettings();
  
  /* Set validators for the control port and IP address fields */
  ui.lineControlAddress->setValidator(new IPValidator(this));
  ui.lineControlPort->setValidator(new QIntValidator(1, 65535, this));
  
  /* Bind event to actions */
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));
  connect(ui.btnBrowseTorDataDirectory, SIGNAL(clicked()),
          this, SLOT(browseTorDataDirectory()));

  /* Hide platform specific features */
#if defined(Q_WS_WIN)
  ui.grpPermissions->setVisible(false);
  ui.grpService->setVisible(TorService::isSupported());
#endif
}

/** Destructor */
AdvancedPage::~AdvancedPage()
{
  delete _settings;
}

/** Saves all settings for this page. */
bool
AdvancedPage::save(QString &errmsg)
{
  QHostAddress controlAddress(ui.lineControlAddress->text());
  if (controlAddress.isNull()) {
    errmsg = tr("'%1' is not a valid IP address.")
               .arg(ui.lineControlAddress->text());
    return false; 
  }
  _settings->setControlAddress(controlAddress);
  _settings->setControlPort(ui.lineControlPort->text().toUShort());
  _settings->setTorrc(ui.lineTorConfig->text());
  _settings->setDataDirectory(ui.lineTorDataDirectory->text());
  _settings->setUser(ui.lineUser->text());
  _settings->setGroup(ui.lineGroup->text());
  
#if defined(Q_WS_WIN)
  /* Install or uninstall the Tor service as necessary */
  setupService(ui.chkUseService->isChecked());
#endif

  return true;
}

/** Loads previously saved settings. */
void
AdvancedPage::load()
{
  ui.lineControlAddress->setText(_settings->getControlAddress().toString());
  ui.lineControlPort->setText(QString::number(_settings->getControlPort()));
  ui.lineTorConfig->setText(_settings->getTorrc());
  ui.lineTorDataDirectory->setText(_settings->getDataDirectory());
  ui.lineUser->setText(_settings->getUser());
  ui.lineGroup->setText(_settings->getGroup());

#if defined(Q_WS_WIN)
  TorService s;
  ui.chkUseService->setChecked(s.isInstalled());
#endif
}

/** Open a QFileDialog to browse for Tor config file. */
void
AdvancedPage::browseTorConfig()
{
  /* Prompt the user to select a file or create a new one */
  QString filename = QFileDialog::getOpenFileName(this, 
                       tr("Select Tor Configuration File"),
                       QFileInfo(ui.lineTorConfig->text()).fileName());
 
  /* Make sure a filename was selected */
  if (filename.isEmpty()) {
    return;
  }

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
    QString errmsg;
    if (!touch_file(filename, false, &errmsg)) {
      VMessageBox::warning(this,
        tr("Failed to Create File"),
        tr("Unable to create %1 [%2]").arg(filename)
                                      .arg(errmsg),
        VMessageBox::Ok);
      return;
    }
  }
  ui.lineTorConfig->setText(filename);
}

/** Opens a QFileDialog for the user to browse to or create a directory for
 * Tor's DataDirectory. */
void
AdvancedPage::browseTorDataDirectory()
{
  QString dataDir = QFileDialog::getExistingDirectory(this,
                      tr("Select a Directory to Use for Tor Data"),
                      ui.lineTorDataDirectory->text());
  
  if (!dataDir.isEmpty()) 
    ui.lineTorDataDirectory->setText(dataDir);
}

#if defined(Q_WS_WIN)
/** Installs or removes the Tor service as necessary. */
void
AdvancedPage::setupService(bool useService)
{
  TorService service;
  bool isInstalled = service.isInstalled();

  if (!useService && isInstalled) {
    /* Uninstall if we don't want to use it anymore */
    Vidalia::torControl()->stop();
    
    if (!service.remove()) {
      VMessageBox::critical(this,
                            tr("Unable to remove Tor Service"),
                            tr("Vidalia was unable to remove the Tor service.\n\n"
                               "You may need to remove it manually."), 
                            VMessageBox::Ok, VMessageBox::Cancel);
    }
  } else if (useService && !isInstalled) {
    /* Install if we want to start using a service */
    if (!service.install(_settings->getExecutable(),
                         _settings->getTorrc(),
                         _settings->getControlPort())) {
      VMessageBox::critical(this,
                            tr("Unable to install Tor Service"),
                            tr("Vidalia was unable to install the Tor service."),
                            VMessageBox::Ok, VMessageBox::Cancel);
    }
  }
}
#endif

