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
 * \brief Tor server configuration options
 */

#include <vidalia.h>
#include <util/net.h>
#include <util/http.h>
#include <util/html.h>
#include <gui/common/vmessagebox.h>

#include "serverpage.h"
#include "ipvalidator.h"
#include "portvalidator.h"
#include "domainvalidator.h"
#include "nicknamevalidator.h"

/** Delay between updating our server IP address (in ms). */
#define AUTO_UPDATE_ADDR_INTERVAL  1000*60*60

/** Help topics */
#define EXIT_POLICY_HELP      "server.exitpolicy"
#define BANDWIDTH_HELP        "server.bandwidth"

/* These are completely made up values (in bytes/sec). */
#define CABLE256_AVG_RATE       (32*1024)
#define CABLE256_MAX_RATE       (64*1024)
#define CABLE512_AVG_RATE       (64*1024)
#define CABLE512_MAX_RATE       (128*1024)
#define CABLE768_AVG_RATE       (96*1024)
#define CABLE768_MAX_RATE       (192*1024)
#define T1_AVG_RATE             (192*1024)
#define T1_MAX_RATE             (384*1024)
#define HIGHBW_AVG_RATE         (3072*1024)
#define HIGHBW_MAX_RATE         (6144*1024)

/** Minimum allowed bandwidth rate (20KB) */
#define MIN_BANDWIDTH_RATE      20
/** Maximum bandwidth rate. This is limited to 2147483646 bytes, 
 * or 2097151 kilobytes. (2147483646/1024) */ 
#define MAX_BANDWIDTH_RATE      2097151


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
  connect(_autoUpdateTimer, SIGNAL(timeout()), 
          this, SLOT(updateServerIP()));
 
  /* Bind events to actions */
  connect(ui.btnGetAddress, SIGNAL(clicked()), this, SLOT(getServerAddress()));
  connect(ui.btnRateHelp, SIGNAL(clicked()), this, SLOT(bandwidthHelp()));
  connect(ui.btnExitHelp, SIGNAL(clicked()), this, SLOT(exitPolicyHelp()));
  connect(ui.cmboRate, SIGNAL(currentIndexChanged(int)),
          this, SLOT(rateChanged(int)));
  connect(ui.lineAvgRateLimit, SIGNAL(editingFinished()), 
                         this, SLOT(customRateChanged()));
  connect(ui.lineMaxRateLimit, SIGNAL(editingFinished()), 
                         this, SLOT(customRateChanged()));

  /* Set validators for address, mask and various port number fields */
  ui.lineServerNickname->setValidator(new NicknameValidator(this));
  ui.lineServerAddress->setValidator(new DomainValidator(this));
  ui.lineServerPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineDirPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineAvgRateLimit->setValidator(
    new QIntValidator(MIN_BANDWIDTH_RATE, MAX_BANDWIDTH_RATE, this));
  ui.lineMaxRateLimit->setValidator(
    new QIntValidator(MIN_BANDWIDTH_RATE, MAX_BANDWIDTH_RATE, this));
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
    _autoUpdateTimer->start(AUTO_UPDATE_ADDR_INTERVAL);
  } else {
    _autoUpdateTimer->stop();
  }
}

/** Saves changes made to settings on the Server settings page. */
bool
ServerPage::save(QString &errmsg)
{
  /* Force the bandwidth rate limits to validate */
  customRateChanged();
  
  if (ui.chkEnableServer->isChecked()) {
    /* A server must have an ORPort and a nickname */
    if (ui.lineServerPort->text().isEmpty() ||
        ui.lineServerNickname->text().isEmpty()) {
      errmsg = tr("You must specify at least a server nickname and port.");
      return false;
    }
    /* If the bandwidth rates aren't set, use some defaults before saving */
    if (ui.lineAvgRateLimit->text().isEmpty()) {
      ui.lineAvgRateLimit->setText(QString::number(2097152/1024) /* 2MB */);
    }
    if (ui.lineMaxRateLimit->text().isEmpty()) {
      ui.lineMaxRateLimit->setText(QString::number(5242880/1024) /* 5MB */);
    }
  }
  _settings->setServerEnabled(ui.chkEnableServer->isChecked());
  _settings->setDirectoryMirror(ui.chkMirrorDirectory->isChecked());
  _settings->setAutoUpdateAddress(ui.chkAutoUpdate->isChecked()); 
  _settings->setNickname(ui.lineServerNickname->text());
  _settings->setORPort(ui.lineServerPort->text().toUInt());
  _settings->setDirPort(ui.lineDirPort->text().toUInt());
  _settings->setAddress(ui.lineServerAddress->text());
  _settings->setContactInfo(ui.lineServerContact->text());
  saveBandwidthLimits();
  setAutoUpdateTimer(ui.chkAutoUpdate->isChecked());

  /* Save exit polices */
//   ExitPolicy exitPolicy;
//   for (int i = 0; i < ui.lstExitPolicies->topLevelItemCount(); ++i) {
//     savePolicy(ui.lstExitPolicies->topLevelItem(i), exitPolicy);
//   }
//   _settings->setExitPolicy(exitPolicy);
  
  /* If we're connected to Tor and we've changed the server settings, attempt
   * to apply the new settings now. */
  if (_torControl->isConnected() && _settings->changedSinceLastApply()) {
    if (!_settings->apply(&errmsg)) {
      _settings->revert();
      return false;
    }
  }
  return true;
}

/** Loads previously saved settings */
void
ServerPage::load()
{
  ui.chkEnableServer->setChecked(_settings->isServerEnabled());
  ui.chkMirrorDirectory->setChecked(_settings->isDirectoryMirror());
  ui.chkAutoUpdate->setChecked(_settings->getAutoUpdateAddress());
  setAutoUpdateTimer(_settings->getAutoUpdateAddress());

  ui.lineServerNickname->setText(_settings->getNickname());
  ui.lineServerPort->setText(QString::number(_settings->getORPort()));
  ui.lineDirPort->setText(QString::number(_settings->getDirPort()));
  ui.lineServerAddress->setText(_settings->getAddress());
  ui.lineServerContact->setText(_settings->getContactInfo());
  loadBandwidthLimits();

//   foreach (Policy policy, _settings->getExitPolicy().policyList()) {
//     addPolicyItem(policy);
//   }
  
  ui.frmServer->setVisible(ui.chkEnableServer->isChecked());
}

/** Adds the exit policy contained in item to the exitPolicy */
// void
// ServerPage::savePolicy(QTreeWidgetItem *item, ExitPolicy &exitPolicy)
// {
//   /* Add policy to ServerSettings */
//   exitPolicy.addPolicy(Policy(item->text(COL_ACTION),
//                               item->text(COL_ADDRESS),
//                               item->text(COL_PORT)));
// }

// /** Adds a new exit policy to the user's configuration */
// void
// ServerPage::addPolicy()
// {
//   /* They must enter a valid address */
//   QString address = ui.lineExitAddress->text();
//   if (!address.isEmpty()) {
//     if (((IPValidator *)ui.lineExitAddress->validator())->
//         validate(address) != QValidator::Acceptable) {
//       return;
//     }
//   }
  
//   /* If port range specified, must be valid */
//   QString fromPort = ui.lineExitFromPort->text();
//   QString toPort = ui.lineExitToPort->text();
  
//   if (!fromPort.isEmpty() && !toPort.isEmpty() && toPort != fromPort) {
//     if (toPort == "*") toPort = "65535";
//     if (fromPort == "*") fromPort = "1";
//     if (fromPort.toUShort() > toPort.toUShort()) {
//       return;
//     }
//   }
  
//   /* Add the policy to the list */
//   addPolicyItem(Policy(Policy::toAction(ui.cmboExitAction->currentText()), 
//                        QHostAddress(ui.lineExitAddress->text()),
//                        ui.lineExitMask->text().toUShort(), 
//                        fromPort.toUShort(),
//                        toPort.toUShort()), false);

//   /* Clear input text boxes */
//   ui.lineExitAddress->clear();
//   ui.lineExitMask->clear();
//   ui.lineExitFromPort->clear();
//   ui.lineExitToPort->clear();
// }

// /** Adds a new QTreeWidget item to the exit policy list */
// void
// ServerPage::addPolicyItem(Policy policy, bool append)
// {
//   QTreeWidgetItem *newPolicy = new QTreeWidgetItem();

//   newPolicy->setText(COL_ACTION,  policy.action());
//   newPolicy->setText(COL_ADDRESS, policy.address());
//   newPolicy->setText(COL_PORT,    policy.ports());
 
//   for (int i = 0; i < newPolicy->columnCount(); i++) {
//     newPolicy->setTextAlignment(i, Qt::AlignHCenter);
//   }

//   if (append) {
//     ui.lstExitPolicies->addTopLevelItem(newPolicy);
//   } else {
//     ui.lstExitPolicies->insertTopLevelItem(0, newPolicy);
//   }
// }

/** Shows exit policy related help information */
void
ServerPage::exitPolicyHelp()
{
  Vidalia::help(EXIT_POLICY_HELP);
}

/** Shows the bandwidth rate limiting help information */
void
ServerPage::bandwidthHelp()
{
  Vidalia::help(BANDWIDTH_HELP);
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
    VMessageBox::warning(this, tr("Error"),
      p(tr("Vidalia was unable to determine your public IP address.")),
      VMessageBox::Ok);
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
    int button = VMessageBox::information(this, tr("Get Address"),
                   tr("Vidalia was only able to find a private IP " 
                      "address for your server.\n\nWould you like to "
                      "access an external service to determine your public " 
                      "IP address?"),
                    VMessageBox::Yes, VMessageBox::No);
    if (button == VMessageBox::Yes) {
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

/** Loads the server's bandwidth average and burst limits. */
void
ServerPage::loadBandwidthLimits()
{
  quint32 avgRate = _settings->getBandwidthAvgRate();
  quint32 maxRate = _settings->getBandwidthBurstRate();

  if (avgRate == CABLE256_AVG_RATE && 
      maxRate == CABLE256_MAX_RATE) {
    /* Cable/DSL 256 Kbps */
    ui.cmboRate->setCurrentIndex(CableDsl256); 
  } else if (avgRate == CABLE512_AVG_RATE && 
             maxRate == CABLE512_MAX_RATE) {
    /* Cable/DSL 512 Kbps */
    ui.cmboRate->setCurrentIndex(CableDsl512);
  } else if (avgRate == CABLE768_AVG_RATE && 
             maxRate == CABLE768_MAX_RATE) {
    /* Cable/DSL 768 Kbps */
    ui.cmboRate->setCurrentIndex(CableDsl768);
  } else if (avgRate == T1_AVG_RATE && 
             maxRate == T1_MAX_RATE) {
    /* T1/Cable/DSL 1.5 Mbps */
    ui.cmboRate->setCurrentIndex(T1CableDsl1500);
  } else if (avgRate == HIGHBW_AVG_RATE && 
             maxRate == HIGHBW_MAX_RATE) {
    /* > 1.5 Mbps */
    ui.cmboRate->setCurrentIndex(GreaterThan1500);
  } else {
    /* Custom bandwidth limits */
    ui.cmboRate->setCurrentIndex(CustomBwLimits);
  }
  /* Fill in the custom bandwidth limit boxes */
  ui.lineAvgRateLimit->setText(QString::number(avgRate/1024));
  ui.lineMaxRateLimit->setText(QString::number(maxRate/1024));
}

/** Saves the server's bandwidth average and burst limits. */
void
ServerPage::saveBandwidthLimits()
{
  quint32 avgRate, maxRate;

  switch (ui.cmboRate->currentIndex()) {
    case CableDsl256: /* Cable/DSL 256 Kbps */
      avgRate = CABLE256_AVG_RATE;
      maxRate = CABLE256_MAX_RATE;
      break;
    case CableDsl512: /* Cable/DSL 512 Kbps */
      avgRate = CABLE512_AVG_RATE;
      maxRate = CABLE512_MAX_RATE;
      break;
    case CableDsl768: /* Cable/DSL 768 Kbps */
      avgRate = CABLE768_AVG_RATE;
      maxRate = CABLE768_MAX_RATE;
      break;
    case T1CableDsl1500: /* T1/Cable/DSL 1.5 Mbps */
      avgRate = T1_AVG_RATE;
      maxRate = T1_MAX_RATE;
      break;
    case GreaterThan1500: /* > 1.5 Mbps */
      avgRate = HIGHBW_AVG_RATE;
      maxRate = HIGHBW_MAX_RATE;
      break;
    default: /* Custom bandwidth limits */
      avgRate = (quint32)(ui.lineAvgRateLimit->text().toUInt()*1024);
      maxRate = (quint32)(ui.lineMaxRateLimit->text().toUInt()*1024);
      break;
  }
  _settings->setBandwidthAvgRate(avgRate);
  _settings->setBandwidthBurstRate(maxRate);
}

/** Called when the user selects a new value from the rate combo box. */
void
ServerPage::rateChanged(int index)
{
  /* If the "Custom" option is selected, show the custom bandwidth 
   * limits form. */
  ui.frmCustomRate->setVisible(index == CustomBwLimits);
}

/** Called when the user edits the long-term average or maximum bandwidth limit. 
 * This ensures that the average bandwidth rate is greater than MIN_RATE
 * (20KB/s) and that the max rate is greater than the average rate. */
void
ServerPage::customRateChanged()
{
  /* Make sure the average rate isn't too low or too high */
  quint32 avgRate = (quint32)ui.lineAvgRateLimit->text().toUInt();
  if (avgRate < MIN_BANDWIDTH_RATE) {
    ui.lineAvgRateLimit->setText(QString::number(MIN_BANDWIDTH_RATE));    
  }
  if (avgRate > MAX_BANDWIDTH_RATE) {
    ui.lineAvgRateLimit->setText(QString::number(MAX_BANDWIDTH_RATE));
  }
  /* Ensure the max burst rate is greater than the average rate but less than
   * the maximum allowed rate. */
  quint32 burstRate = (quint32)ui.lineMaxRateLimit->text().toUInt();
  if (avgRate > burstRate) {
    ui.lineMaxRateLimit->setText(QString::number(avgRate));
  }
  if (burstRate > MAX_BANDWIDTH_RATE) {
    ui.lineMaxRateLimit->setText(QString::number(MAX_BANDWIDTH_RATE));
  }
}

