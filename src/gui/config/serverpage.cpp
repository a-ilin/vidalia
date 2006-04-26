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
 * \file serverpage.cpp
 * \version $Id$
 */

#include <vidalia.h>
#include <util/net.h>
#include <util/http.h>
#include <util/html.h>

#include "serverpage.h"
#include "ipvalidator.h"
#include "portvalidator.h"
#include "domainvalidator.h"

/* Default Exit Policy */
#define DEFAULT_POLICY    Policy(AcceptAll)
/* Columns of the Exit Policy list */
#define COL_ACTION    0
#define COL_ADDRESS   1
#define COL_PORT      2

/** Delay between updating our server IP address (in ms). */
#define AUTO_UPDATE_ADDR_INTERVAL  1000*60*60

/* Help topics */
#define EXIT_HELP     "server.exitpolicy"

/** Constructor */
ServerPage::ServerPage(QWidget *parent)
: ConfigPage(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  
  /* Keep a pointer to the TorControl object used to talk to Tor */
  _torControl = Vidalia::torControl();

  /* Create ServerSettings object */
  _settings = new ServerSettings(_torControl);

  /* Create a timer that we can use to remind ourselves to check if our IP
   * changed since last time we looked. */
  _autoUpdateTimer = new QTimer(this);
    
  /* Bind events to actions */
  connect(ui.btnAddPolicy, SIGNAL(clicked()), this, SLOT(addPolicy()));
  connect(ui.btnRemovePolicy, SIGNAL(clicked()), this, SLOT(removePolicy()));
  connect(ui.btnRaisePriority, SIGNAL(clicked()), this, SLOT(raisePriority()));
  connect(ui.btnLowerPriority, SIGNAL(clicked()), this, SLOT(lowerPriority()));
  connect(ui.btnGetAddress, SIGNAL(clicked()), this, SLOT(getServerAddress()));
  connect(ui.btnExitHelp, SIGNAL(clicked()), this, SLOT(exitHelp()));
  
  /* Set validators for address, mask and various port number fields */
  ui.lineServerAddress->setValidator(new DomainValidator(this));
  ui.lineServerPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineDirPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineExitAddress->setValidator(new IPValidator(this));
  ui.lineExitMask->setValidator(new QIntValidator(0, 32, this));
  ui.lineExitFromPort->setValidator(new PortValidator(this));
  ui.lineExitToPort->setValidator(new PortValidator(this));
}

/** Destructor */
ServerPage::~ServerPage()
{
  delete _settings;
}

/** Enables or disables the automatic IP address update timer. */
void
ServerPage::setAutoUpdateTimer(bool enabled)
{
  if (enabled && _settings->isServerEnabled()) {
    connect(_autoUpdateTimer, SIGNAL(timeout()), 
            this, SLOT(updateServerIP()));
    _autoUpdateTimer->start(AUTO_UPDATE_ADDR_INTERVAL);
  } else {
    _autoUpdateTimer->stop();
  }
}

/** Saves changes made to settings on the Server settings page. */
bool
ServerPage::save(QString &errmsg)
{
  if (ui.chkEnableServer->isChecked() &&
      (ui.lineServerPort->text().isEmpty() ||
       ui.lineServerNickname->text().isEmpty())) {
    errmsg = tr("You must specify at least a server nickname and port.");
    return false;
  }
  _settings->setServerEnabled(ui.chkEnableServer->isChecked());
  _settings->setDirectoryMirror(ui.chkMirrorDirectory->isChecked());
  _settings->setMiddleman(ui.chkMiddleMan->isChecked());
  _settings->setAutoUpdateAddress(ui.chkAutoUpdate->isChecked()); 
  _settings->setNickname(ui.lineServerNickname->text());
  _settings->setORPort(ui.lineServerPort->text().toUInt());
  _settings->setDirPort(ui.lineDirPort->text().toUInt());
  _settings->setAddress(ui.lineServerAddress->text());
  _settings->setContactInfo(ui.lineServerContact->text());
  setAutoUpdateTimer(ui.chkAutoUpdate->isChecked());

  /* Save exit polices */
  ExitPolicy exitPolicy;
  for (int i = 0; i < ui.lstExitPolicies->topLevelItemCount(); ++i) {
    savePolicy(ui.lstExitPolicies->topLevelItem(i), exitPolicy);
  }
  _settings->setExitPolicy(exitPolicy);
  
  bool success = (_torControl->isConnected() ? _settings->apply(&errmsg) : true);
  if (!success) {
    _settings->revert();
  }
  return success;
}

/** Loads previously saved settings */
void
ServerPage::load()
{
  ui.chkEnableServer->setChecked(_settings->isServerEnabled());
  ui.chkMirrorDirectory->setChecked(_settings->isDirectoryMirror());
  ui.chkMiddleMan->setChecked(_settings->isMiddleman());
  ui.chkAutoUpdate->setChecked(_settings->getAutoUpdateAddress());
  setAutoUpdateTimer(_settings->getAutoUpdateAddress());

  ui.lineServerNickname->setText(_settings->getNickname());
  ui.lineServerPort->setText(QString::number(_settings->getORPort()));
  ui.lineDirPort->setText(QString::number(_settings->getDirPort()));
  ui.lineServerAddress->setText(_settings->getAddress());
  ui.lineServerContact->setText(_settings->getContactInfo());
  
  /* Load the exit policies into the list */
  ui.lstExitPolicies->clear();
  
  foreach (Policy policy, _settings->getExitPolicy().policyList()) {
    addPolicyItem(policy);
  }
  
  ui.frmServer->setVisible(ui.chkEnableServer->isChecked());
}

/** Adds the exit policy contained in item to the exitPolicy */
void
ServerPage::savePolicy(QTreeWidgetItem *item, ExitPolicy &exitPolicy)
{
  /* Add policy to ServerSettings */
  exitPolicy.addPolicy(Policy(item->text(COL_ACTION),
                              item->text(COL_ADDRESS),
                              item->text(COL_PORT)));
}

/** Adds a new exit policy to the user's configuration */
void
ServerPage::addPolicy()
{
  /* They must enter a valid address */
  QString address = ui.lineExitAddress->text();
  if (!address.isEmpty()) {
    if (((IPValidator *)ui.lineExitAddress->validator())->
        validate(address) != QValidator::Acceptable) {
      return;
    }
  }
  
  /* If port range specified, must be valid */
  QString fromPort = ui.lineExitFromPort->text();
  QString toPort = ui.lineExitToPort->text();
  
  if (!fromPort.isEmpty() && !toPort.isEmpty() && toPort != fromPort) {
    if (toPort == "*") toPort = "65535";
    if (fromPort == "*") fromPort = "1";
    if (fromPort.toUShort() > toPort.toUShort()) {
      return;
    }
  }
  
  /* Add the policy to the list */
  addPolicyItem(Policy(Policy::toAction(ui.cmboExitAction->currentText()), 
                       QHostAddress(ui.lineExitAddress->text()),
                       ui.lineExitMask->text().toUShort(), 
                       fromPort.toUShort(),
                       toPort.toUShort()));

  /* Clear input text boxes */
  ui.lineExitAddress->clear();
  ui.lineExitMask->clear();
  ui.lineExitFromPort->clear();
  ui.lineExitToPort->clear();
}

/** Adds a new QTreeWidget item to the exit policy list */
void
ServerPage::addPolicyItem(Policy policy)
{
  QTreeWidgetItem *newPolicy = new QTreeWidgetItem();

  newPolicy->setText(COL_ACTION,  policy.action());
  newPolicy->setText(COL_ADDRESS, policy.address());
  newPolicy->setText(COL_PORT,    policy.ports());
 
  for (int i = 0; i < newPolicy->columnCount(); i++) {
    newPolicy->setTextAlignment(i, Qt::AlignHCenter);
  }

  ui.lstExitPolicies->addTopLevelItem(newPolicy);
}

/** Removes selected exit policy from the user's configuration */
void
ServerPage::removePolicy()
{
  int index = selectedIndex();
  
  if (index > -1) {
    delete ui.lstExitPolicies->takeTopLevelItem(index);
  }
}

/** Raises selected exit policy's priority level */
void
ServerPage::raisePriority()
{
  int index = selectedIndex();
  
  if (index > 0) {
      ui.lstExitPolicies->insertTopLevelItem(index - 1, 
                                  ui.lstExitPolicies->takeTopLevelItem(index));
  }
}

/** Lowers selected exit policy's priority level */
void
ServerPage::lowerPriority()
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
ServerPage::selectedIndex()
{
  if (ui.lstExitPolicies->selectedItems().isEmpty()) return -1;
  
  /* This list only contains one element so take it */
  QTreeWidgetItem *selectedItem = ui.lstExitPolicies->selectedItems()[0];
  return ui.lstExitPolicies->indexOfTopLevelItem(selectedItem);
}

/** Shows exit policy related help information */
void
ServerPage::exitHelp()
{
  Vidalia::help(EXIT_HELP);
}

/** Accesses an external site to try to get the user's public IP address. */
void
ServerPage::getServerPublicIP()
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
      p(tr("Vidalia was unable to determine your public IP address.")),
      QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/** Attempts to determine this machine's IP address. If the local IP address
 * is a private address, then the user is asked whether they would like to
 * access an external site to try to get their public IP. */
void
ServerPage::getServerAddress()
{
  QHostAddress addr = net_local_address();
  if (net_is_private_address(addr)) {
    int button = QMessageBox::information(this, tr("Get Address"),
                   p(tr("Vidalia was only able to find a private IP " 
                        "address for your server.\n\nWould you like to "
                        "access an external service to determine your public " 
                        "IP address?")),
                    QMessageBox::Yes, QMessageBox::No);
    if (button == QMessageBox::Yes) {
      getServerPublicIP();
      return;
    }
  } else {
    ui.lineServerAddress->setText(addr.toString());
  }
}

/** Checks to see if this server's public IP had changed. If it has, then
 * update the UI, and Tor (if it's running). */
void
ServerPage::updateServerIP()
{
  bool changed = false;
  QString ip;
  QHostAddress addr = net_local_address();
  
  if (net_is_private_address(addr)) {
    /* Try to get our public IP and see if it changed recently. */
    if (net_get_public_ip(ip) && ip != _settings->getAddress()) {
      changed = true;
    }
  } else if (addr.toString() != _settings->getAddress()) {
    ip = addr.toString();
    changed = true;
  }
  
  if (changed) {
    /* It changed so update our settings and the UI. */
    _settings->setAddress(ip);
    ui.lineServerAddress->setText(ip);

    /* If Tor is running, let it know about the change */
    if (_torControl->isConnected()) {
      _settings->apply();
    }
  }
}

