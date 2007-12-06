/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
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
#include <vmessagebox.h>
#include <file.h>
#include <vidalia.h>

#include "ipvalidator.h"
#include "advancedpage.h"

#if defined(Q_WS_WIN)
#include <control/torservice.h>
#endif


/** Constructor */
AdvancedPage::AdvancedPage(QWidget *parent)
: ConfigPage(parent, tr("Advanced"))
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Create TorSettings object */
  _settings = new TorSettings(Vidalia::torControl());
  
  /* Set validators for the control port and IP address fields */
  ui.lineControlAddress->setValidator(new IPValidator(this));
  ui.lineControlPort->setValidator(new QIntValidator(1, 65535, this));
  
  /* Bind event to actions */
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));
  connect(ui.btnBrowseTorDataDirectory, SIGNAL(clicked()),
          this, SLOT(browseTorDataDirectory()));
  connect(ui.cmbAuthMethod, SIGNAL(currentIndexChanged(int)),
          this, SLOT(authMethodChanged(int)));
  connect(ui.chkRandomPassword, SIGNAL(toggled(bool)),
          ui.linePassword, SLOT(setDisabled(bool)));

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

/** Applies the network configuration settings to Tor. Returns true if the
 * settings were applied successfully. Otherwise, <b>errmsg</b> is set
 * and false is returned. */
bool
AdvancedPage::apply(QString &errmsg)
{
  return _settings->apply(&errmsg);
}

/** Reverts the Tor configuration settings to their values at the last
 * time they were successfully applied to Tor. */
bool
AdvancedPage::changedSinceLastApply()
{
  return _settings->changedSinceLastApply();
}

/** Returns true if the user has changed their advanced Tor settings since
 * the last time they were applied to Tor. */
void
AdvancedPage::revert()
{
  return _settings->revert();
}

/** Saves all settings for this page. */
bool
AdvancedPage::save(QString &errmsg)
{
  /* Validate the control listener address */
  QHostAddress controlAddress(ui.lineControlAddress->text());
  if (controlAddress.isNull()) {
    errmsg = tr("'%1' is not a valid IP address.")
               .arg(ui.lineControlAddress->text());
    return false; 
  }
  
  /* Validate the selected authentication options */
  TorSettings::AuthenticationMethod authMethod = 
    indexToAuthMethod(ui.cmbAuthMethod->currentIndex());
  if (authMethod == TorSettings::PasswordAuth
        && ui.linePassword->text().isEmpty()
        && !ui.chkRandomPassword->isChecked()) {
    errmsg = tr("You selected 'Password' authentication, but did not "
                "specify a password.");
    return false;
  }
 
  /* Only remember the torrc and datadir values if Vidalia started Tor, or
   * if the user changed the displayed values. */
  if (!Vidalia::torControl()->isVidaliaRunningTor()) {
    QString torrc = ui.lineTorConfig->text();
    if (torrc != _settings->getTorrc())
      _settings->setTorrc(torrc);

    QString dataDir = ui.lineTorDataDirectory->text();
    if (dataDir != _settings->getDataDirectory())
      _settings->setDataDirectory(dataDir);
  } else {
    _settings->setTorrc(ui.lineTorConfig->text());
    _settings->setDataDirectory(ui.lineTorDataDirectory->text());
  }

  _settings->setControlAddress(controlAddress);
  _settings->setControlPort(ui.lineControlPort->text().toUShort());
  _settings->setUser(ui.lineUser->text());
  _settings->setGroup(ui.lineGroup->text());
  
  _settings->setAuthenticationMethod(authMethod);
  _settings->setUseRandomPassword(ui.chkRandomPassword->isChecked());
  if (authMethod == TorSettings::PasswordAuth
        && !ui.chkRandomPassword->isChecked())
    _settings->setControlPassword(ui.linePassword->text());

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
  
  ui.cmbAuthMethod->setCurrentIndex(
    authMethodToIndex(_settings->getAuthenticationMethod()));
  ui.chkRandomPassword->setChecked(_settings->useRandomPassword());
  if (!ui.chkRandomPassword->isChecked())
    ui.linePassword->setText(_settings->getControlPassword());

#if defined(Q_WS_WIN)
  TorService s;
  ui.chkUseService->setChecked(s.isInstalled());
#endif
}

/** Called when the user selects a different authentication method from the
 * combo box. */
void
AdvancedPage::authMethodChanged(int index)
{
  bool usePassword = (indexToAuthMethod(index) == TorSettings::PasswordAuth);
  ui.linePassword->setEnabled(usePassword && !ui.chkRandomPassword->isChecked());
  ui.chkRandomPassword->setEnabled(usePassword);
}

/** Returns the authentication method for the given <b>index</b>. */
TorSettings::AuthenticationMethod
AdvancedPage::indexToAuthMethod(int index)
{
  switch (index) {
    case 0: return TorSettings::NullAuth;
    case 1: return TorSettings::CookieAuth;
    case 2: return TorSettings::PasswordAuth;
    default: break;
  }
  return TorSettings::UnknownAuth;
}

/** Returns the index in the authentication methods combo box for the given
 * authentication <b>method</b>. */
int
AdvancedPage::authMethodToIndex(TorSettings::AuthenticationMethod method)
{
  switch (method) {
    case TorSettings::NullAuth: return 0;
    case TorSettings::CookieAuth: return 1;
    default: break;
  }
  return 2;
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

