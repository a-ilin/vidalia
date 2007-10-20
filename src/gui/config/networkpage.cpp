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
 * \file networkpage.cpp
 * \version $Id$
 * \brief Network and firewall configuration options
 */

#include <QMenu>
#include <QIntValidator>
#include <QClipboard>
#include <QHostAddress>
#include <networksettings.h>
#include <vmessagebox.h>
#include <vidalia.h>
#include <util/string.h>

#include "networkpage.h"
#include "domainvalidator.h"

#define IMG_COPY  ":/images/22x22/edit-copy.png"


/** Constructor */
NetworkPage::NetworkPage(QWidget *parent)
: ConfigPage(parent, tr("Network"))
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
 
  connect(ui.btnAddBridge, SIGNAL(clicked()), this, SLOT(addBridge()));
  connect(ui.btnRemoveBridge, SIGNAL(clicked()), this, SLOT(removeBridge()));
  connect(ui.btnCopyBridge, SIGNAL(clicked()), 
          this, SLOT(copySelectedBridgesToClipboard()));
  connect(ui.listBridges, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(bridgeContextMenuRequested(QPoint)));
  connect(ui.listBridges, SIGNAL(itemSelectionChanged()),
          this, SLOT(bridgeSelectionChanged()));
  connect(ui.lineBridge, SIGNAL(returnPressed()), this, SLOT(addBridge()));
  connect(Vidalia::torControl(), SIGNAL(authenticated()),
          this, SLOT(onAuthenticated()));
  connect(Vidalia::torControl(), SIGNAL(disconnected()),
          this, SLOT(onDisconnected()));
  
  ui.lblNoBridgeSupport->setVisible(false);
  ui.lineHttpProxyAddress->setValidator(new DomainValidator(this));
  ui.lineHttpProxyPort->setValidator(new QIntValidator(1, 65535, this));

  vApp->createShortcut(QKeySequence(QKeySequence::Copy),
                       ui.listBridges, this,
                       SLOT(copySelectedBridgesToClipboard()));
}

/** Applies the network configuration settings to Tor. Returns true if the   *
 * settings were applied successfully. Otherwise, <b>errmsg</b> is set and   *
 * false is returned. */
bool
NetworkPage::apply(QString &errmsg)
{
  return NetworkSettings(Vidalia::torControl()).apply(&errmsg);
}

/** Returns true if the user has changed their server settings since the   *
 * last time they were applied to Tor. */
bool
NetworkPage::changedSinceLastApply()
{
  return NetworkSettings(Vidalia::torControl()).changedSinceLastApply();
}

/** Reverts the server configuration settings to their values at the last   *
 * time they were successfully applied to Tor. */
void
NetworkPage::revert()
{
  NetworkSettings(Vidalia::torControl()).revert();
}

/** Called when Vidalia has connected and authenticated to Tor. This will
 * check Tor's version number and, if it's too old, will disable the bridge
 * settings UI and show a message indicating the user's Tor is too old. */
void
NetworkPage::onAuthenticated()
{
  quint32 torVersion = Vidalia::torControl()->getTorVersion();
  if (torVersion < 0x020003) {
    ui.grpBridgeSettings->setEnabled(false);
    ui.lblNoBridgeSupport->setVisible(true);
  }
}

/** Called when Vidalia disconnects from Tor. This will reenable the bridge
 * settings (if they were previously disabled) and hide the warning message
 * indicating the user's Tor does not support bridges. */
void
NetworkPage::onDisconnected()
{
  ui.grpBridgeSettings->setEnabled(true);
  ui.lblNoBridgeSupport->setVisible(false);
}

/** Verifies that <b>bridge</b> is a valid bridge identifier and places a 
 * normalized identifier in <b>out</b>. The normalized identifier will have
 * all spaces removed from the fingerprint portion (if any) and all
 * hexadecimal characters converted to uppercase. Returns true if
 * <b>bridge</b> is a valid bridge identifier, false otherwise. */
bool
NetworkPage::validateBridge(const QString &bridge, QString *out)
{
  QString temp = bridge;
  if (temp.startsWith("bridge ", Qt::CaseInsensitive))
    temp = temp.remove(0, 7); /* remove "bridge " */

  QStringList parts = temp.split(" ", QString::SkipEmptyParts);
  if (parts.isEmpty())
    return false;

  QString s = parts.at(0);
  if (s.contains(":")) {
    if (s.endsWith(":"))
      return false;

    int index = s.indexOf(":");
    QString host = s.mid(0, index);
    QString port = s.mid(index + 1);
    if (QHostAddress(host).isNull()
          || QHostAddress(host).protocol() != QAbstractSocket::IPv4Protocol
          || port.toUInt() < 1 
          || port.toUInt() > 65535)
      return false;
    temp = s;
    if (parts.size() > 1) {
      QString fp = static_cast<QStringList>(parts.mid(1)).join("");
      if (fp.length() != 40 || !string_is_hex(fp))
        return false;
      temp += " " + fp.toUpper();
    }
  } else {
    QString fp = parts.join("");
    if (fp.length() != 40 || !string_is_hex(fp))
      return false;
    temp = fp.toUpper();
  }
  *out = temp;
  return true;
}

/** Adds a bridge to the bridge list box. */
void
NetworkPage::addBridge()
{
  QString bridge;
  QString input = ui.lineBridge->text().trimmed();

  if (input.isEmpty())
    return;
  if (!validateBridge(input, &bridge)) {
    VMessageBox::warning(this,
                  tr("Invalid Bridge"),
                  tr("The specified bridge identifier is not valid."),
                  VMessageBox::Ok|VMessageBox::Default);
    return;
  }
  if (!ui.listBridges->findItems(bridge, Qt::MatchFixedString).isEmpty())
    return; /* duplicate bridge */

  ui.listBridges->addItem(bridge);
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
  ui.btnCopyBridge->setEnabled(!ui.listBridges->selectedItems().isEmpty());
}

/** Saves changes made to settings on the Firewall settings page. */
bool
NetworkPage::save(QString &errmsg)
{
  NetworkSettings settings(Vidalia::torControl());
  QStringList bridgeList;
  QList<quint16> reachablePorts;
  bool ok;
  
  if (ui.chkUseProxy->isChecked()
        && (ui.lineHttpProxyAddress->text().isEmpty()
            || ui.lineHttpProxyPort->text().isEmpty())) {
    errmsg = tr("You must specify both an IP address or hostname and a "
                "port number to configure Tor to use a proxy to access "
                "the Internet.");
    return false;
  }
  if (ui.chkFascistFirewall->isChecked()
        && ui.lineReachablePorts->text().isEmpty()) {
    errmsg = tr("You must specify one or more ports to which your "
                "firewall allows you to connect.");
    return false;
  }

  /* Save the HTTP/HTTPS proxy settings */
  settings.setUseHttpProxy(ui.chkUseProxy->isChecked());
  settings.setUseHttpsProxy(ui.chkProxyUseHttps->isChecked());
  if (!ui.lineHttpProxyAddress->text().isEmpty()) {
    QString proxy = ui.lineHttpProxyAddress->text();
    if (!ui.lineHttpProxyPort->text().isEmpty())
      proxy += ":" + ui.lineHttpProxyPort->text();

    settings.setHttpProxy(proxy);
    settings.setHttpsProxy(proxy);
  } else {
    settings.setHttpProxy("");
    settings.setHttpsProxy("");
  }

  if (!ui.lineHttpProxyUsername->text().isEmpty() ||
      !ui.lineHttpProxyPassword->text().isEmpty()) {
    QString auth = ui.lineHttpProxyUsername->text() + ":" +
                   ui.lineHttpProxyPassword->text();
    settings.setHttpProxyAuthenticator(auth);
    settings.setHttpsProxyAuthenticator(auth);
  } else {
    settings.setHttpProxyAuthenticator("");
    settings.setHttpsProxyAuthenticator("");
  }
  
  /* Save the reachable port settings */
  settings.setFascistFirewall(ui.chkFascistFirewall->isChecked());
  foreach (QString portString,
           ui.lineReachablePorts->text().split(",", QString::SkipEmptyParts)) {
    quint32 port = portString.toUInt(&ok);
    if (!ok || port < 1 || port > 65535) {
      errmsg = tr("'%1' is not a valid port number.").arg(portString);
      return false;
    }
    reachablePorts << (quint16)port;
  }
  settings.setReachablePorts(reachablePorts);

  /* Save the bridge settings */
  settings.setUseBridges(ui.chkUseBridges->isChecked());
  for (int i = 0; i < ui.listBridges->count(); i++)
    bridgeList << ui.listBridges->item(i)->text();
  settings.setBridgeList(bridgeList);

  return true;
}

/** Loads previously saved settings */
void
NetworkPage::load()
{
  NetworkSettings settings(Vidalia::torControl());
  QStringList reachablePortStrings;

  /* Load HTTP/HTTPS proxy settings */
  ui.chkUseProxy->setChecked(settings.getUseHttpProxy());
  ui.chkProxyUseHttps->setChecked(settings.getUseHttpsProxy());
  QStringList proxy = settings.getHttpProxy().split(":");
  QStringList proxyAuth = settings.getHttpProxyAuthenticator().split(":");
  if (proxy.size() >= 1)
    ui.lineHttpProxyAddress->setText(proxy.at(0));
  if (proxy.size() >= 2)
    ui.lineHttpProxyPort->setText(proxy.at(1));
  if (proxyAuth.size() >= 1)  
    ui.lineHttpProxyUsername->setText(proxyAuth.at(0));
  if (proxyAuth.size() >= 2)
    ui.lineHttpProxyPassword->setText(proxyAuth.at(1));

  /* Load firewall settings */
  ui.chkFascistFirewall->setChecked(settings.getFascistFirewall());
  QList<quint16> reachablePorts = settings.getReachablePorts();
  foreach (quint16 port, reachablePorts) {
    reachablePortStrings << QString::number(port);
  }
  ui.lineReachablePorts->setText(reachablePortStrings.join(","));

  /* Load bridge settings */
  ui.chkUseBridges->setChecked(settings.getUseBridges() 
                                || settings.getTunnelDirConns());
  ui.listBridges->clear();
  ui.listBridges->addItems(settings.getBridgeList());
}

