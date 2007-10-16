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
#include <gui/common/vmessagebox.h>

#include "serverpage.h"
#include "ipvalidator.h"
#include "portvalidator.h"
#include "domainvalidator.h"
#include "nicknamevalidator.h"


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

/** Ports represented by the "Websites" checkbox. (80) */
#define PORTS_HTTP   (QStringList() << "80")
/** Ports represented by the "Secure Websites" checkbox. (443) */
#define PORTS_HTTPS  (QStringList() << "443")
/** Ports represented by the "Retrieve Mail" checkbox. (110,143,993,995) */
#define PORTS_MAIL   (QStringList() << "110" << "143" << "993" << "995")
/** Ports represented by the "Instant Messaging" checkbox.
 * (703,1863,5050,5190,5222,8300,8888) */
#define PORTS_IM     (QStringList() << "706" << "1863" << "5050" << "5190" \
                                    << "5222" << "8300" << "8888")
/** Ports represented by the "Internet Relay Chat" checkbox. 
 * (6660-6669,6697) */
#define PORTS_IRC    (QStringList() << "6660-6669" << "6697")


/** Constructor */
ServerPage::ServerPage(QWidget *parent)
: ConfigPage(parent, tr("Server"))
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  
  /* Keep a pointer to the TorControl object used to talk to Tor */
  _torControl = Vidalia::torControl();

  /* Create ServerSettings object */
  _settings = new ServerSettings(_torControl);

  /* Bind events to actions */
  connect(ui.btnRateHelp, SIGNAL(clicked()), this, SLOT(bandwidthHelp()));
  connect(ui.btnExitHelp, SIGNAL(clicked()), this, SLOT(exitPolicyHelp()));
  connect(ui.cmboRate, SIGNAL(currentIndexChanged(int)),
          this, SLOT(rateChanged(int)));
  connect(ui.lineAvgRateLimit, SIGNAL(editingFinished()), 
                         this, SLOT(customRateChanged()));
  connect(ui.lineMaxRateLimit, SIGNAL(editingFinished()), 
                         this, SLOT(customRateChanged()));
  connect(ui.rdoClientMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));
  connect(ui.rdoServerMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));
  connect(ui.rdoBridgeMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));

  /* Set validators for address, mask and various port number fields */
  ui.lineServerNickname->setValidator(new NicknameValidator(this));
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

/** Called when the user toggles any one of the server mode radio buttons
 * and hides or displays the server configuration tabs appropriatey. */
void
ServerPage::serverModeChanged(bool enabled)
{
  enabled = (ui.rdoServerMode->isChecked() || ui.rdoBridgeMode->isChecked());
  ui.tabsMenu->setVisible(enabled);
}

/** Returns true if the user has changed their server settings since the
 * last time they were applied to Tor. */
bool
ServerPage::changedSinceLastApply()
{
  return _settings->changedSinceLastApply();
}

/** Applies the server configuration settings to Tor. Returns true if the
 * settings were applied successfully. Otherwise, <b>errmsg</b> is
 * set and false is returned. */
bool
ServerPage::apply(QString &errmsg)
{
  return _settings->apply(&errmsg);
}

/** Returns true if the user has changed their server settings since the
 * last time they were applied to Tor. */
void
ServerPage::revert()
{
  _settings->revert();
}

/** Saves changes made to settings on the Server settings page. */
bool
ServerPage::save(QString &errmsg)
{
  /* Force the bandwidth rate limits to validate */
  customRateChanged();
  
  if (ui.rdoServerMode->isChecked() || ui.rdoBridgeMode->isChecked()) {
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
  
  /* "Server" is enabled whether we're a bridge or normal relay. "Bridge" is
   * only enabled if we're a bridge (obviously). */
  _settings->setServerEnabled(ui.rdoServerMode->isChecked()
                                || ui.rdoBridgeMode->isChecked());
  _settings->setBridgeEnabled(ui.rdoBridgeMode->isChecked());
  
  /* Save the rest of the server settings. */
  _settings->setDirectoryMirror(ui.chkMirrorDirectory->isChecked());
  _settings->setNickname(ui.lineServerNickname->text());
  _settings->setORPort(ui.lineServerPort->text().toUInt());
  _settings->setDirPort(ui.lineDirPort->text().toUInt());
  _settings->setContactInfo(ui.lineServerContact->text());
  saveBandwidthLimits();
  saveExitPolicies();
  
  return true;
}

/** Loads previously saved settings */
void
ServerPage::load()
{
  if (_settings->isBridgeEnabled())
    ui.rdoBridgeMode->setChecked(true);
  else if (_settings->isServerEnabled())
    ui.rdoServerMode->setChecked(true);
  else
    ui.rdoClientMode->setChecked(true);

  ui.lineServerNickname->setText(_settings->getNickname());
  ui.lineServerPort->setText(QString::number(_settings->getORPort()));
  ui.lineDirPort->setText(QString::number(_settings->getDirPort()));
  ui.lineServerContact->setText(_settings->getContactInfo());
  ui.chkMirrorDirectory->setChecked(_settings->isDirectoryMirror());
  
  loadBandwidthLimits();
  loadExitPolicies();

  //ui.frmServer->setVisible(ui.chkEnableServer->isChecked());
}

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

/** */
void
ServerPage::loadExitPolicies()
{
  ExitPolicy exitPolicy = _settings->getExitPolicy();
  
  if (exitPolicy.contains(Policy(Policy::RejectAll))) {
    /* If the policy ends with reject *:*, check if the policy explicitly
     * accepts these ports */
    ui.chkWebsites->setChecked(exitPolicy.acceptsPorts(PORTS_HTTP));
    ui.chkSecWebsites->setChecked(exitPolicy.acceptsPorts(PORTS_HTTPS));
    ui.chkMail->setChecked(exitPolicy.acceptsPorts(PORTS_MAIL));
    ui.chkIRC->setChecked(exitPolicy.acceptsPorts(PORTS_IRC));
    ui.chkIM->setChecked(exitPolicy.acceptsPorts(PORTS_IM));
    ui.chkMisc->setChecked(false);
  } else {
    /* If the exit policy ends with accept *:*, check if the policy explicitly
     * rejects these ports */
    ui.chkWebsites->setChecked(!exitPolicy.rejectsPorts(PORTS_HTTP));
    ui.chkSecWebsites->setChecked(!exitPolicy.rejectsPorts(PORTS_HTTPS));
    ui.chkMail->setChecked(!exitPolicy.rejectsPorts(PORTS_MAIL));
    ui.chkIRC->setChecked(!exitPolicy.rejectsPorts(PORTS_IRC));
    ui.chkIM->setChecked(!exitPolicy.rejectsPorts(PORTS_IM));
    ui.chkMisc->setChecked(true);
  }
}

/** */
void
ServerPage::saveExitPolicies()
{
  ExitPolicy exitPolicy;
  bool rejectUnchecked = ui.chkMisc->isChecked();
  
  /* If misc is checked, then reject unchecked items and leave the default exit
   * policy alone. Else, accept only checked items and end with reject *:*,
   * replacing the default exit policy. */
  if (ui.chkWebsites->isChecked() && !rejectUnchecked) {
    exitPolicy.addAcceptedPorts(PORTS_HTTP);
  } else if (!ui.chkWebsites->isChecked() && rejectUnchecked) {
    exitPolicy.addRejectedPorts(PORTS_HTTP);
  }
  if (ui.chkSecWebsites->isChecked() && !rejectUnchecked) {
    exitPolicy.addAcceptedPorts(PORTS_HTTPS);
  } else if (!ui.chkSecWebsites->isChecked() && rejectUnchecked) {
    exitPolicy.addRejectedPorts(PORTS_HTTPS);
  }
  if (ui.chkMail->isChecked() && !rejectUnchecked) {
    exitPolicy.addAcceptedPorts(PORTS_MAIL);
  } else if (!ui.chkMail->isChecked() && rejectUnchecked) {
    exitPolicy.addRejectedPorts(PORTS_MAIL);
  }
  if (ui.chkIRC->isChecked() && !rejectUnchecked) {
    exitPolicy.addAcceptedPorts(PORTS_IRC);
  } else if (!ui.chkIRC->isChecked() && rejectUnchecked) {
    exitPolicy.addRejectedPorts(PORTS_IRC);
  }
  if (ui.chkIM->isChecked() && !rejectUnchecked) {
    exitPolicy.addAcceptedPorts(PORTS_IM);
  } else if (!ui.chkIM->isChecked() && rejectUnchecked) {
    exitPolicy.addRejectedPorts(PORTS_IM);
  }
  if (!ui.chkMisc->isChecked()) {
    exitPolicy.addPolicy(Policy(Policy::RejectAll));
  }
  _settings->setExitPolicy(exitPolicy);
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

