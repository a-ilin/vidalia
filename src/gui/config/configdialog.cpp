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
 * \file configdialog.cpp
 * \version $Id$
 */

#include <QMessageBox>

#include <util/net.h>
#include <util/http.h>

#include "configdialog.h"
#include "portvalidator.h"
#include "ipvalidator.h"

/* Page indices in the QListWidget */
#define PAGE_GENERAL  0
#define PAGE_SERVER   1
#define PAGE_ADVANCED 2

/* Columns of the Exit Policy list */
#define COL_ACTION    0
#define COL_ADDRESS   1
#define COL_PORT      2

/* Help topics */
#define EXIT_HELP     "server.exitpolicy"

/** Constructor */
ConfigDialog::ConfigDialog(TorControl *torControl,
                           HelpBrowser *helpBrowser,
                           QWidget* parent)
: QDialog(parent)
{
  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Set validators for address, mask and various port number fields */
  ui.lineControlPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineServerPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineDirPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineExitAddress->setValidator(new IPValidator(this));
  ui.lineExitMask->setValidator(new QIntValidator(0, 32, this));
  ui.lineExitFromPort->setValidator(new PortValidator(this));
  ui.lineExitToPort->setValidator(new PortValidator(this));
  
  /* Keep a pointer to the TorControl object used to talk to Tor */
  _torControl = torControl;

  /* Keep a pointer to the HelpBrowser object used to show help */
  _helpBrowser = helpBrowser;
  
  /* Create necessary ConfigDialog QObjects */
  _vidaliaSettings = new VidaliaSettings();
  _serverSettings = new ServerSettings(_torControl);
  _torSettings = new TorSettings();

  /* Bind events to actions */
  createActions();

  /* Set General Settings selected */
  ui.lstPages->setItemSelected(ui.lstPages->item(PAGE_GENERAL), true);

  /* Also hide platform specific features where necessary */
#ifdef Q_WS_WIN
  ui.grpPermissions->setVisible(false);
#else
  ui.chkRunWithSys->setVisible(false);
#endif
}

/** Destructor */
ConfigDialog::~ConfigDialog()
{
  delete _vidaliaSettings;
  delete _torSettings;
  delete _serverSettings;
}

/** Overloads the default show so we can load settings */
void
ConfigDialog::show()
{
  /* Load saved settings */
  loadSettings();

  if (!this->isVisible()) {
    QDialog::show();
  } else {
    QDialog::activateWindow();
    QDialog::raise();
  }
}

/** Connects actions to events. */
void
ConfigDialog::createActions()
{
  connect(ui.lstPages, 
          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this, SLOT(changePage(QListWidgetItem *, QListWidgetItem *)));

  connect(ui.btnAddPolicy, SIGNAL(clicked()), this, SLOT(addPolicy()));
  connect(ui.btnRemovePolicy, SIGNAL(clicked()), this, SLOT(removePolicy()));
  connect(ui.btnRaisePriority, SIGNAL(clicked()), this, SLOT(raisePriority()));
  connect(ui.btnLowerPriority, SIGNAL(clicked()), this, SLOT(lowerPriority()));
  connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(cancelChanges()));
  connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(ui.btnBrowseTorPath, SIGNAL(clicked()), this, SLOT(browseTorPath()));
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));
  connect(ui.btnGetAddress, SIGNAL(clicked()), this, SLOT(getServerAddress()));
  connect(ui.btnExitHelp, SIGNAL(clicked()), this, SLOT(exitHelp()));
}

/** Adds a new exit policy to the user's configuration */
void
ConfigDialog::addPolicy()
{
  /* They have to at least enter something as an IP address */
  if (ui.lineExitAddress->text().isEmpty()) {
    return;
  }
  
  QTreeWidgetItem *newPolicy = new QTreeWidgetItem();
  
  /* Add new policy's action */
  newPolicy->setText(COL_ACTION, ui.cmboExitAction->currentText());
  newPolicy->setTextAlignment(COL_ACTION, Qt::AlignCenter);

  /* Add new policy's ip address */
  QString address = ui.lineExitAddress->text();
  if (!ui.lineExitMask->text().isEmpty()) {
    address += ":" + ui.lineExitMask->text();
  }  
  newPolicy->setText(COL_ADDRESS, address);
  newPolicy->setTextAlignment(COL_ADDRESS, Qt::AlignCenter);
  
  /* Add new policy's port or port range */
  QString portRange = ui.lineExitFromPort->text();
  if (!portRange.isEmpty()) {
    QString toPort = ui.lineExitToPort->text();
    if (!toPort.isEmpty() && portRange != "*") {
      portRange += "-" + ui.lineExitToPort->text();
    }
  }
  newPolicy->setText(COL_PORT, portRange);
  newPolicy->setTextAlignment(COL_PORT, Qt::AlignCenter);
  
  /* Add new policy to list */
  ui.lstExitPolicies->addTopLevelItem(newPolicy);

  /* Clear input text boxes */
  ui.lineExitAddress->clear();
  ui.lineExitMask->clear();
  ui.lineExitFromPort->clear();
  ui.lineExitToPort->clear();
}

/** Removes selected exit policy from the user's configuration */
void
ConfigDialog::removePolicy()
{
  int index = selectedIndex();
  
  if (index > -1) {
    ui.lstExitPolicies->takeTopLevelItem(index);
  }
}

/** Raises selected exit policy's priority level */
void
ConfigDialog::raisePriority()
{
  int index = selectedIndex();
  
  if (index > 0) {
      ui.lstExitPolicies->insertTopLevelItem(index - 1, 
                                  ui.lstExitPolicies->takeTopLevelItem(index));
  }
}

/** Lowers selected exit policy's priority level */
void
ConfigDialog::lowerPriority()
{
  int index = selectedIndex();
  int lastItem = ui.lstExitPolicies->topLevelItemCount() - 1;

  if (index > -1 && index < lastItem) {
    ui.lstExitPolicies->insertTopLevelItem(index + 1, 
                                  ui.lstExitPolicies->takeTopLevelItem(index));
  }
}

/** Returns the index of the selected list item, -1 if none selected */
int
ConfigDialog::selectedIndex()
{
  /* This list only contains one element so take it */
  QTreeWidgetItem *selectedItem = ui.lstExitPolicies->selectedItems()[0];

  if (selectedItem) {
    return ui.lstExitPolicies->indexOfTopLevelItem(selectedItem);
  }
  return -1;
}

/** Shows exit policy related help information */
void
ConfigDialog::exitHelp()
{
  _helpBrowser->showTopic(EXIT_HELP);
}

/** Changes settings page. */
void
ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current) {
    current = previous;
  }
  
  ui.stackPages->setCurrentIndex(ui.lstPages->row(current));
}

/** Loads the saved ConfigDialog settings. */
void
ConfigDialog::loadSettings()
{
  /* Toggle Server config frame */
  showServerConfig(ui.chkEnableServer->isChecked());
  
  /* Load General settings */
  loadGeneralSettings();
  /* Load Server settings */
  loadServerSettings();
  /* Load Advanced settings */
  loadAdvancedSettings();
}

/** Toggle display of server configuration settings frame */
void
ConfigDialog::showServerConfig(bool show)
{
  ui.frmServer->setVisible(show);
}

/** Load and display settings for the General settings page. */
void
ConfigDialog::loadGeneralSettings()
{
  ui.lineTorPath->setText(_torSettings->getPath());
  ui.chkRunTor->setChecked(_vidaliaSettings->runTorAtStart());
  ui.chkRunWithSys->setChecked(_vidaliaSettings->runVidaliaOnBoot());
}

/** Save changes made to settings on the General settings page. */
void
ConfigDialog::saveGeneralSettings()
{
  _torSettings->setPath(ui.lineTorPath->text());
  _vidaliaSettings->setRunTorAtStart(ui.chkRunTor->isChecked());
  _vidaliaSettings->setRunVidaliaOnBoot(ui.chkRunWithSys->isChecked());
}

/** Load and display settings for the Advanced settings page. */
void
ConfigDialog::loadAdvancedSettings()
{
  ui.lineControlPort->setText(QString::number(_torSettings->getControlPort()));
  ui.lineTorConfig->setText(_torSettings->getTorrc());
  ui.lineUser->setText(_torSettings->getUser());
  ui.lineGroup->setText(_torSettings->getGroup());
}

/** Save changes made to settings on the Advanced settings page. */
void
ConfigDialog::saveAdvancedSettings()
{
  _torSettings->setControlPort(ui.lineControlPort->text().toUShort());
  _torSettings->setTorrc(ui.lineTorConfig->text());
  _torSettings->setUser(ui.lineUser->text());
  _torSettings->setGroup(ui.lineGroup->text());
}

/** Load and display settings for the Server settings page. */
void
ConfigDialog::loadServerSettings()
{
  ui.chkEnableServer->setChecked(_serverSettings->isServerEnabled());
  ui.chkMirrorDirectory->setChecked(_serverSettings->isDirectoryMirror());
  ui.chkMiddleMan->setChecked(_serverSettings->isMiddleman());
  ui.lineServerNickname->setText(_serverSettings->getNickname());
  ui.lineServerPort->setText(QString::number(_serverSettings->getORPort()));
  ui.lineDirPort->setText(QString::number(_serverSettings->getDirPort()));
  ui.lineServerAddress->setText(_serverSettings->getAddress());
  ui.lineServerContact->setText(_serverSettings->getContactInfo());
}

/** Saves changes made to settings on the Server settings page. */
bool
ConfigDialog::saveServerSettings(QString *errmsg)
{
  if (ui.chkEnableServer->isChecked() &&
      (ui.lineServerPort->text().isEmpty() ||
       ui.lineServerNickname->text().isEmpty())) {
    *errmsg = tr("You must specify at least a server nickname and port.");
    return false;
  }
  _serverSettings->setServerEnabled(ui.chkEnableServer->isChecked());
  _serverSettings->setDirectoryMirror(ui.chkMirrorDirectory->isChecked());
  _serverSettings->setMiddleman(ui.chkMiddleMan->isChecked());
  _serverSettings->setNickname(ui.lineServerNickname->text());
  _serverSettings->setORPort(ui.lineServerPort->text().toUInt());
  _serverSettings->setDirPort(ui.lineDirPort->text().toUInt());
  _serverSettings->setAddress(ui.lineServerAddress->text());
  _serverSettings->setContactInfo(ui.lineServerContact->text());
  return (_torControl->isConnected() ? _serverSettings->apply(errmsg) : true);    
}

/** Attempts to determine this machine's IP address. If the local IP address
 * is a private address, then the user is asked whether they would like to
 * access an external site to try to get their public IP. */
void
ConfigDialog::getServerAddress()
{
  QHostAddress addr = net_local_address();
  if (net_is_private_address(addr)) {
    int button = QMessageBox::information(this, tr("Get Address"),
                   tr("Vidalia was only able to find a private IP " 
                      "address for your server.\nWould you like to "
                      "access an external service to determine your public " 
                      "IP address?"),
                    QMessageBox::Yes, QMessageBox::No);
    if (button == QMessageBox::Yes) {
      getServerPublicIP();
      return;
    }
  } else {
    ui.lineServerAddress->setText(addr.toString());
  }
}

/** Accesses an external site to try to get the user's public IP address. */
void
ConfigDialog::getServerPublicIP()
{
  QString ip;
  bool success;

  /* This could take a bit, so show the wait cursor. */
  QApplication::setOverrideCursor(Qt::WaitCursor);
  success = net_get_public_ip(ip);
  QApplication::restoreOverrideCursor();
  
  /* Handle the result */
  if (success) {
    ui.lineServerAddress->setText(ip);
  } else {
    QMessageBox::warning(this, tr("Error"),
      tr("Viadlia was uanble to determine your public IP address."),
      QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/** Cancels changes made to settings. */
void
ConfigDialog::cancelChanges()
{
  /* Reload saved settings and exit */
  loadSettings();
  
  QDialog::close();
}

/** Saves changes made to settings. */
void
ConfigDialog::saveChanges()
{
  QString errmsg;
  
  /* Save the settings and exit */
  saveGeneralSettings();
  saveAdvancedSettings();
 
  /* Try to save the user's server settings. If something goes awry, then
   * notify the user, don't save their settings, and show them the offending
   * page. */
  if (!saveServerSettings(&errmsg)) {
    QMessageBox::warning(this, 
      tr("Error Saving Server Configuration"),
      tr("Vidalia encountered an error applying your "
         "server configuration.\n\n") + errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
    _serverSettings->revert();
    ui.lstPages->setCurrentRow(PAGE_SERVER);
    return;
  }
  QDialog::close();
}

/** Open a QFileDialog to browse for Tor executable. */
void 
ConfigDialog::browseTorPath()
{
  QString filename = QDir::convertSeparators(
                          QFileDialog::getExistingDirectory(this,
                              tr("Select Path to Tor"), 
                              ui.lineTorPath->text()));
  if (!filename.isEmpty()) {
    ui.lineTorPath->setText(filename);
  }
}

/** Open a QFileDialog to browse for Tor config file. */
void
ConfigDialog::browseTorConfig()
{
  QString filename = QDir::convertSeparators(
                       QFileDialog::getOpenFileName(this,
                         tr("Select Tor Configuration File")));
  if (!filename.isEmpty()) {
    ui.lineTorConfig->setText(filename);
  }
}

