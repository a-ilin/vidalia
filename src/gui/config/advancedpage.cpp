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
#include <gui/common/vmessagebox.h>
#include <util/file.h>
#include <vidalia.h>
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

  /* Bind event to actions */
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));

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
  Q_UNUSED(errmsg);
  _settings->setControlPort(ui.lineControlPort->text().toUShort());
  _settings->setTorrc(ui.lineTorConfig->text());
  _settings->setUser(ui.lineUser->text());
  _settings->setGroup(ui.lineGroup->text());
  
#if defined(Q_WS_WIN)
  /* Install or uninstall the Tor service as necessary */
  setupService();
#endif

  return true;
}

/** Loads previously saved settings. */
void
AdvancedPage::load()
{
  ui.lineControlPort->setText(QString::number(_settings->getControlPort()));
  ui.lineTorConfig->setText(_settings->getTorrc());
  ui.lineUser->setText(_settings->getUser());
  ui.lineGroup->setText(_settings->getGroup());

#if defined(Q_WS_WIN)
  ui.chkUseService->setChecked(useService());
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

#if defined(Q_WS_WIN)
/** Returns true if service support is enabled and functional */
bool
AdvancedPage::useService()
{
  bool use = false;

  /* If we think we're supposed to be using a service we'd better make
     sure that the service still actually exists since the last time we checked.
  */
  
  if (_settings->getUseService()) {
    TorService* s = new TorService(_settings->getExecutable(),
                                   _settings->getTorrc());
    use = s->isInstalled();
    delete s;

    /* No point in trying to use a broken or non-existent service */
    if (!use) _settings->setUseService(false);
  }
  return use;
}

/** Installs or removes the Tor service as necessary. */
void
AdvancedPage::setupService()
{
  bool checked = ui.chkUseService->isChecked();
  TorService* s = new TorService(_settings->getExecutable(),
                                 _settings->getTorrc());

  if (!checked && s->isInstalled()) {
    /* Uninstall if we don't want to use it anymore */
    Vidalia::torControl()->stop();
    
    if (!s->remove()) {
      VMessageBox::critical(this,
                            tr("Unable to remove Tor Service"),
                            tr("Vidalia was unable to remove the Tor service.\n\n"
                               "You may need to remove it manually."), 
                            VMessageBox::Ok, VMessageBox::Cancel);
    }
    _settings->setUseService(false);

  } else if (checked && !s->isInstalled()) {
    /* Install if we want to start using a service */
    if (!s->install()) {
      VMessageBox::critical(this,
                            tr("Unable to install Tor Service"),
                            tr("Vidalia was unable to install the Tor service."),
                            VMessageBox::Ok, VMessageBox::Cancel);
    }
    _settings->setUseService(s->isInstalled());
  } else {
    _settings->setUseService(checked);
  }

  delete s;
}
#endif

