/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/*
** \file ServerPage.cpp
** \brief Tor server configuration options
*/

#include "config.h"
#include "ServerPage.h"
#include "Vidalia.h"
#include "VMessageBox.h"
#include "ConfigDialog.h"
#include "IpValidator.h"
#include "PortValidator.h"
#include "DomainValidator.h"
#include "NicknameValidator.h"
#include "BridgeUsageDialog.h"

#include "html.h"
#include "stringutil.h"

#if defined(USE_MINIUPNPC)
#include "UPNPTestDialog.h"
#endif


#include <QClipboard>
#include <QMessageBox>

/* These are completely made up values (in bytes/sec). */
#define CABLE256_AVG_RATE       (32*1024)
#define CABLE256_MAX_RATE       (64*1024)
#define CABLE512_AVG_RATE       (64*1024)
#define CABLE512_MAX_RATE       (128*1024)
#define CABLE768_AVG_RATE       (96*1024)
#define CABLE768_MAX_RATE       (192*1024)
#define T1_AVG_RATE             (192*1024)
#define T1_MAX_RATE             (384*1024)
#define HIGHBW_AVG_RATE         (5120*1024)
#define HIGHBW_MAX_RATE         (10240*1024)
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
                                    << "5222" << "5223" << "8300" << "8888")
/** Ports represented by the "Internet Relay Chat" checkbox. 
 * (6660-6669,6697,7000-7001) */
#define PORTS_IRC    (QStringList() << "6660-6669" << "6697" << "7000-7001")


/** Constructor */
ServerPage::ServerPage(QWidget *parent)
: ConfigPage(parent, "Server")
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  
  /* Create ServerSettings object */
  _settings = new ServerSettings(Vidalia::torControl());

  /* Bind events to actions */
  connect(ui.btnRateHelp, SIGNAL(clicked()), this, SLOT(bandwidthHelp()));
  connect(ui.btnExitHelp, SIGNAL(clicked()), this, SLOT(exitPolicyHelp()));
  connect(ui.btnUpnpHelp, SIGNAL(clicked()), this, SLOT(upnpHelp()));
  connect(ui.cmboRate, SIGNAL(currentIndexChanged(int)),
                 this, SLOT(rateChanged(int)));
  connect(ui.lineAvgRateLimit, SIGNAL(editingFinished()), 
                         this, SLOT(customRateChanged()));
  connect(ui.lineMaxRateLimit, SIGNAL(editingFinished()), 
                         this, SLOT(customRateChanged()));
  connect(ui.rdoClientMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));
  connect(ui.rdoNonExitMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));
  connect(ui.rdoServerMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));
  connect(ui.rdoBridgeMode, SIGNAL(toggled(bool)),
                      this, SLOT(serverModeChanged(bool)));
  connect(Vidalia::torControl(), SIGNAL(authenticated()),
                           this, SLOT(onAuthenticated()));
  connect(Vidalia::torControl(), SIGNAL(disconnected()),
                           this, SLOT(onDisconnected()));
  connect(ui.btnCopyBridgeIdentity, SIGNAL(clicked()),
                              this, SLOT(copyBridgeIdentity()));
  connect(ui.lblBridgeUsage, SIGNAL(linkActivated(QString)),
                       this, SLOT(linkActivated(QString)));
  connect(ui.lblWhatsThis, SIGNAL(linkActivated(QString)),
                       this, SLOT(linkActivated(QString)));

  /* Set validators for address, mask and various port number fields */
  ui.lineServerNickname->setValidator(new NicknameValidator(this));
  ui.lineServerPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineDirPort->setValidator(new QIntValidator(1, 65535, this));
  ui.lineAvgRateLimit->setValidator(
    new QIntValidator(MIN_BANDWIDTH_RATE, MAX_BANDWIDTH_RATE, this));
  ui.lineMaxRateLimit->setValidator(
    new QIntValidator(MIN_BANDWIDTH_RATE, MAX_BANDWIDTH_RATE, this));

#if defined(USE_MINIUPNPC)
  connect(ui.btnTestUpnp, SIGNAL(clicked()), this, SLOT(testUpnp()));
#else
  ui.chkEnableUpnp->setVisible(false);
  ui.btnTestUpnp->setVisible(false);
#endif

  _tmpDirPort = "9030";
  _tmpMirror = true;
}

/** Destructor */
ServerPage::~ServerPage()
{
  delete _settings;
}

/** Called when the user changes the UI translation. */
void
ServerPage::retranslateUi()
{
  ui.retranslateUi(this);
}

/** Called when Vidalia has authenticated to Tor. If the user's Tor is not
 * recent enough, this disables the bridge server option and displays a
 * warning if the user had previously configured Tor as a bridge. */
void
ServerPage::onAuthenticated()
{
  quint32 torVersion = Vidalia::torControl()->getTorVersion();
  if (torVersion < 0x020008) {
    ui.rdoBridgeMode->setEnabled(false);
    if (ui.rdoBridgeMode->isChecked()) {
      int ret = VMessageBox::warning(this,
                  tr("Bridge Support Unavailable"),
                  p(tr("You have configured Tor to act as a bridge relay "
                       "for censored users, but your version of Tor does not "
                       "support bridges.")) +
                  p(tr("Please upgrade your Tor software or configure Tor to "
                       "act as a normal Tor relay.")),
                  VMessageBox::ShowSettings|VMessageBox::Default,
                  VMessageBox::Cancel);
      if (ret == VMessageBox::ShowSettings) {
        ConfigDialog *dialog = dynamic_cast<ConfigDialog *>(window());
        if (dialog)
          dialog->showWindow(ConfigDialog::Server);
      }
    }
  }
}

/** Called when Vidalia disconnects from Tor. This method reenables the bridge
 * server option. */
void
ServerPage::onDisconnected()
{
  ui.rdoBridgeMode->setEnabled(true);
}

/** Copies the user's bridge relay identity to the clipboard. */
void
ServerPage::copyBridgeIdentity()
{
  QString bridge = ui.lblBridgeIdentity->text();
  if (!bridge.isEmpty())
    vApp->clipboard()->setText(bridge);
}

/** Loads the user's bridge relay identity into the appropriate widgets. If
 * the user's bridge is not running, then "Not Running" will be displayed.
 * Otherwise, either the bridge's "address:port", "fingerprint", or
 * "address:port fingerprint" will be displayed, depending on whether our
 * GETCONF and GETINFO commands are successful. */
void
ServerPage::loadBridgeIdentity()
{
  TorControl *tc = Vidalia::torControl();
  QString bridge, address, orPort, fingerprint;

  if (tc->isConnected()) {
    tc->getInfo("address", address);
    tc->getInfo("fingerprint", fingerprint);
    tc->getConf("ORPort", orPort);
  
    if (!address.isEmpty() && !orPort.isEmpty() && orPort != "0")
      bridge = address + ":" + orPort + " ";
    if (!fingerprint.isEmpty())
      bridge += fingerprint;
    bridge = bridge.trimmed();
  }

  ui.lblBridgeIdentity->setText(bridge.isEmpty()
                                  ? tr("Your bridge relay is not running.")
                                  : bridge);
  ui.lblYourBridgeRelayIs->setEnabled(!bridge.isEmpty());
  ui.lblBridgeIdentity->setEnabled(!bridge.isEmpty());
  ui.btnCopyBridgeIdentity->setEnabled(!bridge.isEmpty());
}

/** Called when the user toggles any one of the server mode radio buttons
 * and hides or displays the server configuration tabs appropriately. */
void
ServerPage::serverModeChanged(bool enabled)
{
  Q_UNUSED(enabled);
  bool bridgeEnabled = ui.rdoBridgeMode->isChecked();
  bool relayEnabled = ui.rdoServerMode->isChecked() ||
                      ui.rdoNonExitMode->isChecked();
  
  /* Show the tab menu only if the user is running a normal relay or a bridge
   * relay. */
  ui.tabsMenu->setVisible(relayEnabled || bridgeEnabled);
  
  /* Display the widgets that show the user their bridge identity if bridge
   * relay mode is selected. */
  ui.lblYourBridgeRelayIs->setVisible(bridgeEnabled);
  ui.lblBridgeIdentity->setVisible(bridgeEnabled);
  ui.btnCopyBridgeIdentity->setVisible(bridgeEnabled);
  ui.chkPublishBridgeAddress->setVisible(bridgeEnabled);
  ui.lblBridgeUsage->setVisible(bridgeEnabled
                                  && Vidalia::torControl()->isConnected());

  if(bridgeEnabled) {
    if(ui.lineDirPort->text().length() != 0) {
      _tmpDirPort = ui.lineDirPort->text();
      _tmpMirror = ui.chkMirrorDirectory->isChecked();
    }
    ui.lineDirPort->clear();
    ui.chkMirrorDirectory->setChecked(false);
  } else {
    ui.lineDirPort->setText(_tmpDirPort);
    ui.chkMirrorDirectory->setChecked(_tmpMirror);
  }

  ui.lineDirPort->setEnabled(!bridgeEnabled);
  ui.lblDirPort->setEnabled(!bridgeEnabled);

  ui.chkMirrorDirectory->setEnabled(!bridgeEnabled);

  /* Disable the Exit Policies tab when bridge or non-exit relay mode is 
   * selected */
  ui.tabsMenu->setTabEnabled(2, !bridgeEnabled and !ui.rdoNonExitMode->isChecked());

  if(ui.chkMirrorDirectory->isChecked()) {
    ui.lblDirPort->setEnabled(!bridgeEnabled);
    ui.lineDirPort->setEnabled(!bridgeEnabled);
  }
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
  
  if (ui.rdoServerMode->isChecked() || 
      ui.rdoNonExitMode->isChecked() ||
      ui.rdoBridgeMode->isChecked()) {
    /* A server must have an ORPort and a nickname */
    if (ui.lineServerPort->text().isEmpty() ||
        ui.lineServerNickname->text().isEmpty()) {
      errmsg = tr("You must specify at least a relay nickname and port.");
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
                                || ui.rdoNonExitMode->isChecked()
                                || ui.rdoBridgeMode->isChecked());
  _settings->setNonExitEnabled(ui.rdoNonExitMode->isChecked());
  _settings->setBridgeEnabled(ui.rdoBridgeMode->isChecked());
  if (ui.rdoBridgeMode->isChecked())
    _settings->setPublishServerDescriptor(ui.chkPublishBridgeAddress->isChecked());

  /* Save the rest of the server settings. */
  _settings->setNickname(ui.lineServerNickname->text());
  _settings->setORPort(ui.lineServerPort->text().toUInt());
  if (!ui.rdoBridgeMode->isChecked()) {
    _settings->setDirPort(ui.lineDirPort->text().toUInt());
    _settings->setDirectoryMirror(ui.chkMirrorDirectory->isChecked());
  } else {
    _settings->setDirectoryMirror(false);
  }
  _settings->setContactInfo(ui.lineServerContact->text());
  saveBandwidthLimits();
  saveExitPolicies();

#if defined(USE_MINIUPNPC)
  _settings->setUpnpEnabled(ui.chkEnableUpnp->isChecked());
#endif

  return true;
}

/** Loads previously saved settings */
void
ServerPage::load()
{
  if (_settings->isBridgeEnabled())
    ui.rdoBridgeMode->setChecked(true);
  else if (_settings->isNonExitEnabled()) {
    if(_settings->getExitPolicy().toString() != "reject *:*")
      ui.rdoServerMode->setChecked(true);
    else
      ui.rdoNonExitMode->setChecked(true);
  } else if (_settings->isServerEnabled()) {
    if(_settings->getExitPolicy().toString() == "reject *:*")
      ui.rdoNonExitMode->setChecked(true);
    else
      ui.rdoServerMode->setChecked(true);
  } else
    ui.rdoClientMode->setChecked(true);

  ui.lineServerNickname->setText(_settings->getNickname());
  ui.lineServerPort->setText(QString::number(_settings->getORPort()));
  ui.lineDirPort->setText(QString::number(_settings->getDirPort()));
  ui.lineServerContact->setText(_settings->getContactInfo());
  ui.chkMirrorDirectory->setChecked(_settings->isDirectoryMirror());
  ui.lblBridgeUsage->setVisible(_settings->isBridgeEnabled()
                                  && Vidalia::torControl()->isConnected());
  ui.chkPublishBridgeAddress->setChecked(_settings->publishServerDescriptor());

  loadBandwidthLimits();
  loadExitPolicies();
  loadBridgeIdentity();

#if defined(USE_MINIUPNPC)
  ui.chkEnableUpnp->setChecked(_settings->isUpnpEnabled());
#endif
}

/** Shows exit policy related help information */
void
ServerPage::exitPolicyHelp()
{
  emit helpRequested("server.exitpolicy");
}

/** Shows the bandwidth rate limiting help information */
void
ServerPage::bandwidthHelp()
{
  emit helpRequested("server.bandwidth");
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
  ExitPolicy *exitPolicy;
  if(ui.rdoNonExitMode->isChecked()) {
    exitPolicy = new ExitPolicy(ExitPolicy::Middleman);
  } else {
    exitPolicy = new ExitPolicy();
    bool rejectUnchecked = ui.chkMisc->isChecked();
    
    /* If misc is checked, then reject unchecked items and leave the default exit
    * policy alone. Else, accept only checked items and end with reject *:*,
    * replacing the default exit policy. */
    if (ui.chkWebsites->isChecked() && !rejectUnchecked) {
      exitPolicy->addAcceptedPorts(PORTS_HTTP);
    } else if (!ui.chkWebsites->isChecked() && rejectUnchecked) {
      exitPolicy->addRejectedPorts(PORTS_HTTP);
    }
    if (ui.chkSecWebsites->isChecked() && !rejectUnchecked) {
      exitPolicy->addAcceptedPorts(PORTS_HTTPS);
    } else if (!ui.chkSecWebsites->isChecked() && rejectUnchecked) {
      exitPolicy->addRejectedPorts(PORTS_HTTPS);
    }
    if (ui.chkMail->isChecked() && !rejectUnchecked) {
      exitPolicy->addAcceptedPorts(PORTS_MAIL);
    } else if (!ui.chkMail->isChecked() && rejectUnchecked) {
      exitPolicy->addRejectedPorts(PORTS_MAIL);
    }
    if (ui.chkIRC->isChecked() && !rejectUnchecked) {
      exitPolicy->addAcceptedPorts(PORTS_IRC);
    } else if (!ui.chkIRC->isChecked() && rejectUnchecked) {
      exitPolicy->addRejectedPorts(PORTS_IRC);
    }
    if (ui.chkIM->isChecked() && !rejectUnchecked) {
      exitPolicy->addAcceptedPorts(PORTS_IM);
    } else if (!ui.chkIM->isChecked() && rejectUnchecked) {
      exitPolicy->addRejectedPorts(PORTS_IM);
    }
    if (!ui.chkMisc->isChecked()) {
      exitPolicy->addPolicy(Policy(Policy::RejectAll));
    }
  }
  _settings->setExitPolicy(*exitPolicy);
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

/** Tests automatic port forwarding using UPnP. */
void
ServerPage::testUpnp()
{
#if defined(USE_MINIUPNPC)
  UPNPTestDialog dlg(ui.lineServerPort->text().toUInt(),
                     ui.lineDirPort->text().toUInt(), this);
  
  connect(&dlg, SIGNAL(help()), this, SLOT(upnpHelp()));

  dlg.exec();
#endif
}

/** Called when the user clicks the UPnP test dialog's help button. */
void
ServerPage::upnpHelp()
{
  emit helpRequested("server.upnp");
}

/** Called when the user clicks on a QLabel containing a hyperlink. */
void
ServerPage::linkActivated(const QString &url)
{
  if (!url.compare("#bridgeUsage"))
    displayBridgeUsage();
  else if(!url.compare("#bridgeHelp"))
    emit helpRequested("bridges.about");
}

/** Retrieves bridge usage history from Tor, parses and validates it, and
 * then displays it in a new dialog. */
void
ServerPage::displayBridgeUsage()
{
  QString info;
  QMessageBox dlg(this);

  info = Vidalia::torControl()->getInfo("status/clients-seen").toString();
  if (info.isEmpty()) {
    goto none;
  } else {
    QDateTime timeStarted;
    QHash<QString,int> countrySummary;
    QHash<QString,QString> keyvals;
    BridgeUsageDialog dlg(this);
    bool ok;

    keyvals = string_parse_keyvals(info, &ok);
    if (!ok || !keyvals.contains("TimeStarted") 
            || !keyvals.contains("CountrySummary"))
      goto err;

    timeStarted = QDateTime::fromString(keyvals.value("TimeStarted"), 
                                        "yyyy-MM-dd HH:mm:ss");
    if (!timeStarted.isValid())
      goto err;

    // Default is LocalTime, force UTC
    timeStarted.setTimeSpec(Qt::UTC);

    QStringList summary = keyvals.value("CountrySummary")
                                 .split(",", QString::SkipEmptyParts);
    if (summary.isEmpty()) {
      goto none;
    } else {
      foreach (QString pair, summary) {
        QStringList parts = pair.split("=");
        if (parts.size() != 2)
          goto err;

        countrySummary.insert(parts.at(0).toUpper(), parts.at(1).toInt(&ok));
        if (!ok)
          goto err;
      }

      dlg.update(timeStarted, countrySummary);
      dlg.exec();
    }
  }
  return;

none:
  dlg.setIcon(QMessageBox::Information);
  dlg.setWindowTitle(tr("No Recent Usage"));
  dlg.setText(tr("No clients have used your relay recently."));
  dlg.setInformativeText(tr("Leave your relay running so clients have "
                            "a better chance of finding and using it."));
  dlg.setStandardButtons(QMessageBox::Ok);
  dlg.exec();
  return;

err:
  dlg.setIcon(QMessageBox::Warning);
  dlg.setWindowTitle(tr("Bridge History"));
  dlg.setText(tr("Vidalia was unable to retrieve your bridge's usage "
                 "history."));
  dlg.setInformativeText(tr("Tor returned an improperly formatted "
                            "response when Vidalia requested your "
                            "bridge's usage history."));
  dlg.setDetailedText(tr("The returned response was: %1").arg(info));
  dlg.setStandardButtons(QMessageBox::Ok);
  dlg.exec();
}

