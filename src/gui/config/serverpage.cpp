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

#include "serverpage.h"
#include "ipvalidator.h"
#include "portvalidator.h"

/* Default Exit Policy */
#define DEFAULT_POLICY    Policy(AcceptAll)
/* Columns of the Exit Policy list */
#define COL_ACTION    0
#define COL_ADDRESS   1
#define COL_PORT      2

/* Help topics */
#define EXIT_HELP     "server.exitpolicy"

/** Constructor */
ServerPage::ServerPage(TorControl *torControl, QWidget *parent)
: ConfigPage(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Keep a pointer to the TorControl object used to talk to Tor */
  _torControl = torControl;

  /* Create ServerSettings object */
  _settings = new ServerSettings(_torControl);

  /* Bind events to actions */
  connect(ui.btnAddPolicy, SIGNAL(clicked()), this, SLOT(addPolicy()));
  connect(ui.btnRemovePolicy, SIGNAL(clicked()), this, SLOT(removePolicy()));
  connect(ui.btnRaisePriority, SIGNAL(clicked()), this, SLOT(raisePriority()));
  connect(ui.btnLowerPriority, SIGNAL(clicked()), this, SLOT(lowerPriority()));
  connect(ui.btnGetAddress, SIGNAL(clicked()), this, SLOT(getServerAddress()));
  connect(ui.btnExitHelp, SIGNAL(clicked()), this, SLOT(exitHelp()));
  
  /* Set validators for address, mask and various port number fields */
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
  _settings->setNickname(ui.lineServerNickname->text());
  _settings->setORPort(ui.lineServerPort->text().toUInt());
  _settings->setDirPort(ui.lineDirPort->text().toUInt());
  _settings->setAddress(ui.lineServerAddress->text());
  _settings->setContactInfo(ui.lineServerContact->text());
  _settings->setOverridePolicy(ui.chkExitOverride->isChecked());

  /* Remove/Add necessary default exit rule */
  setDefaultRule();
  
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
  
  ui.lineServerNickname->setText(_settings->getNickname());
  ui.lineServerPort->setText(QString::number(_settings->getORPort()));
  ui.lineDirPort->setText(QString::number(_settings->getDirPort()));
  ui.lineServerAddress->setText(_settings->getAddress());
  ui.lineServerContact->setText(_settings->getContactInfo());
  ui.chkExitOverride->setChecked(_settings->getOverridePolicy());
  
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
  /* Build policy string */
  QString policyString = item->text(COL_ACTION) + " ";
  policyString += item->text(COL_ADDRESS) + ":" + item->text(COL_PORT);
  
  /* Add policy to ServerSettings */
  exitPolicy.addPolicy(Policy(policyString));
}

/** Moves or appends the correct default exit rule to the policy list */
void
ServerPage::setDefaultRule()
{
  bool override = ui.chkExitOverride->isChecked();
  Policy::SpecialPolicy action;
  QString actionString;
  QList<QTreeWidgetItem *> list;
  bool found = false;
  
  if (override) {
    action = Policy::RejectAll;
    actionString = "reject";
  } else {
    action = Policy::AcceptAll;
    actionString = "accept";
  }
  
  /* Search for the policy, if exists: move to bottom else: append it */
  /* Remove any of the opposite default exit policy */
  list = ui.lstExitPolicies->findItems("0.0.0.0", Qt::MatchExactly, COL_ADDRESS);
  
  foreach (QTreeWidgetItem *item, list) {
    if (item->text(COL_PORT) == "*") {
      int index = ui.lstExitPolicies->indexOfTopLevelItem(item);
      
      /* Found target so move to bottom of list */
      if (item->text(COL_ACTION) == actionString) {
        ui.lstExitPolicies->addTopLevelItem(ui.lstExitPolicies->
                                                      takeTopLevelItem(index));
        found = true;
        
      /* Found the opposite so remove */
      } else {
        ui.lstExitPolicies->takeTopLevelItem(index);
      }
    }
  }

  /* Search failed so just append the necessary policy */
  if (!found) addPolicyItem(Policy(action));
}

/** Adds a new exit policy to the user's configuration */
void
ServerPage::addPolicy()
{
  /* They must enter a valid address */
  QString address = ui.lineExitAddress->text();
  int i;
  if (ui.lineExitAddress->validator()->
      validate(address, i) != QValidator::Acceptable) {
    return;
  }
  
  /* Add the policy to the list */
  addPolicyItem(ui.cmboExitAction->currentText(), ui.lineExitAddress->text(),
                ui.lineExitMask->text(), ui.lineExitFromPort->text(),
                ui.lineExitToPort->text());

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
  /* Convert to strings */
  QString action = (policy.action() == Policy::Accept ? "accept" : "reject");
  QHostAddress address = policy.address();
  QString addr = (address.isNull() ? "*" : address.toString());
  QString mask = (policy.mask() ? QString::number(policy.mask()) : "");
  int port = policy.fromPort();
  QString fromPort = (port ? QString::number(port) : "");
  port = policy.toPort();
  QString toPort = (port ? QString::number(port) : "");

  /* Add to list */
  addPolicyItem(action, addr, mask, fromPort, toPort);
}

/** Adds a new QTreeWidget item to the exit policy list */
void
ServerPage::addPolicyItem(QString action, QString address, QString mask,
                          QString fromPort, QString toPort)
{
  QTreeWidgetItem *newPolicy = new QTreeWidgetItem();
  
  newPolicy->setText(COL_ACTION, action);
  newPolicy->setTextAlignment(COL_ACTION, Qt::AlignCenter);
  
  if (!mask.isEmpty()) {
    address += "/" + mask;
  }
  newPolicy->setText(COL_ADDRESS, address);
  newPolicy->setTextAlignment(COL_ADDRESS, Qt::AlignCenter);

  if (!fromPort.isEmpty()) {
    if (!toPort.isEmpty() && fromPort != "*") {
      fromPort += "-" + toPort;
    }
  } else {
    fromPort = "*";
  }

  newPolicy->setText(COL_PORT, fromPort);
  newPolicy->setTextAlignment(COL_PORT, Qt::AlignCenter);

  ui.lstExitPolicies->addTopLevelItem(newPolicy);
}

/** Removes selected exit policy from the user's configuration */
void
ServerPage::removePolicy()
{
  int index = selectedIndex();
  
  if (index > -1) {
    ui.lstExitPolicies->takeTopLevelItem(index);
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
      tr("Viadlia was uanble to determine your public IP address."),
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

