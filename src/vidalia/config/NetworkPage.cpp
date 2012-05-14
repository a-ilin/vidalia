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
** \file NetworkPage.cpp
** \brief Network and firewall configuration options
*/

#include "NetworkPage.h"
#include "NetworkSettings.h"
#include "VMessageBox.h"
#include "Vidalia.h"
#include "DomainValidator.h"

#include "stringutil.h"

#include <QMenu>
#include <QIntValidator>
#include <QClipboard>
#include <QHostAddress>
#include <QRegExp>
#include <QMessageBox>

#define IMG_COPY  ":/images/22x22/edit-copy.png"


/** Constructor */
NetworkPage::NetworkPage(QWidget *parent)
: ConfigPage(parent, "Network")
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  _settings = new NetworkSettings(Vidalia::torControl());

  connect(ui.btnAddBridge, SIGNAL(clicked()), this, SLOT(addBridge()));
  connect(ui.btnRemoveBridge, SIGNAL(clicked()), this, SLOT(removeBridge()));
  connect(ui.btnCopyBridge, SIGNAL(clicked()),
          this, SLOT(copySelectedBridgesToClipboard()));
  connect(ui.listBridges, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(bridgeContextMenuRequested(QPoint)));
  connect(ui.listBridges, SIGNAL(itemSelectionChanged()),
          this, SLOT(bridgeSelectionChanged()));
  connect(ui.lineBridge, SIGNAL(returnPressed()), this, SLOT(addBridge()));
  connect(ui.lblHelpFindBridges, SIGNAL(linkActivated(QString)),
          this, SLOT(onLinkActivated(QString)));
  connect(ui.cmboProxyType, SIGNAL(currentIndexChanged(int)),
          this, SLOT(proxyTypeChanged(int)));

  ui.lineProxyAddress->setValidator(new DomainValidator(this));
  ui.lineProxyPort->setValidator(new QIntValidator(1, 65535, this));

  vApp->createShortcut(QKeySequence(QKeySequence::Copy),
                       ui.listBridges, this,
                       SLOT(copySelectedBridgesToClipboard()));

#if defined(Q_WS_MAC)
  /* On OS X, the network page looks better without frame titles. Everywhere
   * else needs titles or else there's a break in the frame border. */
  ui.grpProxySettings->setTitle("");
  ui.grpFirewallSettings->setTitle("");
  ui.grpBridgeSettings->setTitle("");
#endif
}

/** Default destructor */
NetworkPage::~NetworkPage()
{
  delete _settings;
}

/** Called when the user changes the UI translation. */
void
NetworkPage::retranslateUi()
{
  ui.retranslateUi(this);
}

/** Applies the network configuration settings to Tor. Returns true if the   *
 * settings were applied successfully. Otherwise, <b>errmsg</b> is set and   *
 * false is returned. */
bool
NetworkPage::apply(QString &errmsg)
{
  return _settings->apply(&errmsg);
}

/** Returns true if the user has changed their server settings since the   *
 * last time they were applied to Tor. */
bool
NetworkPage::changedSinceLastApply()
{
  return _settings->changedSinceLastApply();
}

/** Reverts the server configuration settings to their values at the last   *
 * time they were successfully applied to Tor. */
void
NetworkPage::revert()
{
  _settings->revert();
}

/** Called when a link in a label is clicked. <b>url</b> is the target of
 * the clicked link. */
void
NetworkPage::onLinkActivated(const QString &url)
{
  emit helpRequested(url);
}

/** Adds a bridge to the bridge list box. */
void
NetworkPage::addBridge()
{
  QString input = ui.lineBridge->text().trimmed();

  if (input.isEmpty())
    return;
  if (!ui.listBridges->findItems(input, Qt::MatchFixedString).isEmpty())
    return; /* duplicate bridge */

  ui.listBridges->addItem(input);
  ui.lineBridge->clear();
}

/** Removes one or more selected bridges from the bridge list box. */
void
NetworkPage::removeBridge()
{
  qDeleteAll(ui.listBridges->selectedItems());
}

/** Copies all selected bridges to the clipboard. */
void
NetworkPage::copySelectedBridgesToClipboard()
{
  QString contents;

  foreach (QListWidgetItem *item, ui.listBridges->selectedItems()) {
#if defined(Q_WS_WIN)
    contents += item->text() + "\r\n";
#else
    contents += item->text() + "\n";
#endif
  }
  if (!contents.isEmpty())
    vApp->clipboard()->setText(contents.trimmed());
}

/** Called when the user right-clicks on a bridge and displays a context
 * menu. */
void
NetworkPage::bridgeContextMenuRequested(const QPoint &pos)
{
  QMenu menu(this);

  QListWidgetItem *item = ui.listBridges->itemAt(pos);
  if (!item)
    return;

  QAction *copyAction =
    new QAction(QIcon(IMG_COPY), tr("Copy (Ctrl+C)"), &menu);
  connect(copyAction, SIGNAL(triggered()),
          this, SLOT(copySelectedBridgesToClipboard()));

  menu.addAction(copyAction);
  menu.exec(ui.listBridges->mapToGlobal(pos));
}

/** Called when the user changes which bridges they have selected. */
void
NetworkPage::bridgeSelectionChanged()
{
  bool enabled = !ui.listBridges->selectedItems().isEmpty();
  ui.btnCopyBridge->setEnabled(enabled);
  ui.btnRemoveBridge->setEnabled(enabled);
}

/** Saves changes made to settings on the Firewall settings page. */
bool
NetworkPage::save(QString &errmsg)
{
  QString addr;
  QString user, pass;
  NetworkSettings::ProxyType proxy = NetworkSettings::NoProxy;
  QStringList bridgeList;
  QList<quint16> reachablePorts;
  bool ok;

  if (ui.chkUseProxy->isChecked()) {
    if (ui.lineProxyAddress->text().isEmpty()
          || ui.lineProxyPort->text().isEmpty()) {
      errmsg = tr("You must specify both an IP address or hostname and a "
                  "port number to configure Tor to use a proxy to access "
                  "the Internet.");
      return false;
    }
    if (ui.cmboProxyType->currentIndex() < 0) {
      errmsg = tr("You must select the proxy type.");
      return false;
    }
  }
  if (ui.chkFascistFirewall->isChecked()
        && ui.lineReachablePorts->text().isEmpty()) {
    errmsg = tr("You must specify one or more ports to which your "
                "firewall allows you to connect.");
    return false;
  }

  if (ui.chkUseProxy->isChecked()) {
    if (!ui.lineProxyAddress->text().isEmpty()) {
      addr = ui.lineProxyAddress->text();
      if (!ui.lineProxyPort->text().isEmpty())
        addr += ":" + ui.lineProxyPort->text();
    }

    user = ui.lineProxyUsername->text();
    pass = ui.lineProxyPassword->text();

    QVariant data;
    int type;

    data = ui.cmboProxyType->itemData(ui.cmboProxyType->currentIndex());
    Q_ASSERT(data.isValid());
    type = data.toInt();
    Q_ASSERT(type >= NetworkSettings::ProxyTypeMin &&
             type <= NetworkSettings::ProxyTypeMax);
    proxy = static_cast<NetworkSettings::ProxyType>(type);
  }

  _settings->setProxyType(proxy);
  _settings->setProxyAddress(addr);
  _settings->setProxyUsername(user);
  _settings->setProxyPassword(pass);

  /* Save the reachable port settings */
  _settings->setFascistFirewall(ui.chkFascistFirewall->isChecked());
  foreach (QString portString,
           ui.lineReachablePorts->text().split(",", QString::SkipEmptyParts)) {
    quint32 port = portString.toUInt(&ok);
    if (!ok || port < 1 || port > 65535) {
      errmsg = tr("'%1' is not a valid port number.").arg(portString);
      return false;
    }
    reachablePorts << (quint16)port;
  }
  _settings->setReachablePorts(reachablePorts);

  if (ui.chkUseBridges->isChecked()) {
    if (ui.listBridges->count() < 1) {
      errmsg = tr("You must specify one or more bridges.");
      return false;
    }
  }

  /* Save the bridge settings */
  _settings->setUseBridges(ui.chkUseBridges->isChecked());
  for (int i = 0; i < ui.listBridges->count(); i++)
    bridgeList << ui.listBridges->item(i)->text();
  _settings->setBridgeList(bridgeList);

  return true;
}

/** Loads previously saved settings */
void
NetworkPage::load()
{
  QStringList reachablePortStrings;
  NetworkSettings::ProxyType proxyType;

  /* Load proxy settings */
  proxyType = _settings->getProxyType();
  ui.chkUseProxy->setChecked(proxyType != NetworkSettings::NoProxy);
  QStringList proxy = _settings->getProxyAddress().split(":");
  if (proxy.size() >= 1)
    ui.lineProxyAddress->setText(proxy.at(0));
  if (proxy.size() >= 2)
    ui.lineProxyPort->setText(proxy.at(1));
  ui.lineProxyUsername->setText(_settings->getProxyUsername());
  ui.lineProxyPassword->setText(_settings->getProxyPassword());

  /* SOCKS options are only available on Tor >= 0.2.2.1-alpha, so don't show
   * them if Tor is running and its version is less than that. */
  ui.cmboProxyType->clear();
  if (!vApp->torControl()->isRunning()
        || vApp->torControl()->getTorVersion() >= 0x020201) {
    ui.cmboProxyType->addItem(tr("SOCKS 4"), NetworkSettings::Socks4Proxy);
    ui.cmboProxyType->addItem(tr("SOCKS 5"), NetworkSettings::Socks5Proxy);
  } else if (proxyType == NetworkSettings::Socks4Proxy
              || proxyType == NetworkSettings::Socks5Proxy) {
    /* Disable proxy if the settings include a SOCKS proxy and our version of
     * Tor is not compatible. */
    proxyType = NetworkSettings::NoProxy;
    ui.chkUseProxy->setChecked(false);
  }
  ui.cmboProxyType->addItem(tr("HTTP / HTTPS"),
                            NetworkSettings::HttpHttpsProxy);

  ui.cmboProxyType->setCurrentIndex(ui.cmboProxyType->findData(proxyType));

  /* Load firewall settings */
  ui.chkFascistFirewall->setChecked(_settings->getFascistFirewall());
  QList<quint16> reachablePorts = _settings->getReachablePorts();
  foreach (quint16 port, reachablePorts) {
    reachablePortStrings << QString::number(port);
  }
  ui.lineReachablePorts->setText(reachablePortStrings.join(","));

  /* Load bridge settings */
  ui.chkUseBridges->setChecked(_settings->getUseBridges());
  ui.listBridges->clear();
  ui.listBridges->addItems(_settings->getBridgeList());
}

/** Disable proxy username and password fields when the user wants to use
 * a SOCKS 4 proxy. */
void
NetworkPage::proxyTypeChanged(int selection)
{
  QVariant data = ui.cmboProxyType->itemData(selection);

  if (data.isValid()
      && data.toInt() == NetworkSettings::Socks4Proxy) {
    ui.lineProxyUsername->setEnabled(false);
    ui.lineProxyPassword->setEnabled(false);
  } else {
    ui.lineProxyUsername->setEnabled(true);
    ui.lineProxyPassword->setEnabled(true);
  }
}

