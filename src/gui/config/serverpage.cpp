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

#include <util/net.h>
#include <util/http.h>

#include "serverpage.h"
#include "ipvalidator.h"
#include "portvalidator.h"

/* Columns of the Exit Policy list */
#define COL_ACTION    0
#define COL_ADDRESS   1
#define COL_PORT      2

/* Help topics */
#define EXIT_HELP     "server.exitpolicy"

/** Constructor */
ServerPage::ServerPage(TorControl *torControl, HelpBrowser *browser, QWidget *parent)
: QWidget(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Keep a pointer to the TorControl object used to talk to Tor */
  _torControl = torControl;

  /* Keep a pointer to the HelpBrowser object used to show help */
  _browser = browser;
  
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
ServerPage::saveChanges(QString *errmsg)
{
  if (ui.chkEnableServer->isChecked() &&
      (ui.lineServerPort->text().isEmpty() ||
       ui.lineServerNickname->text().isEmpty())) {
    *errmsg = tr("You must specify at least a server nickname and port.");
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
  bool success = (_torControl->isConnected() ? _settings->apply(errmsg) : true);
  if (!success) {
    _settings->revert();
  }
  return success;
}

/* Loads previously saved settings */
void
ServerPage::loadSettings()
{
  ui.chkEnableServer->setChecked(_settings->isServerEnabled());
  ui.chkMirrorDirectory->setChecked(_settings->isDirectoryMirror());
  ui.chkMiddleMan->setChecked(_settings->isMiddleman());
  ui.lineServerNickname->setText(_settings->getNickname());
  ui.lineServerPort->setText(QString::number(_settings->getORPort()));
  ui.lineDirPort->setText(QString::number(_settings->getDirPort()));
  ui.lineServerAddress->setText(_settings->getAddress());
  ui.lineServerContact->setText(_settings->getContactInfo());

  ui.frmServer->setVisible(ui.chkEnableServer->isChecked());
}

/** Adds a new exit policy to the user's configuration */
void
ServerPage::addPolicy()
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
  /* This list only contains one element so take it */
  QTreeWidgetItem *selectedItem = ui.lstExitPolicies->selectedItems()[0];

  if (selectedItem) {
    return ui.lstExitPolicies->indexOfTopLevelItem(selectedItem);
  }
  return -1;
}

/** Shows exit policy related help information */
void
ServerPage::exitHelp()
{
  _browser->showTopic(EXIT_HELP);
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

