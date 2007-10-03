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

#include <QIntValidator>
#include <vidalia.h>
#include <config/networksettings.h>

#include "networkpage.h"
#include "domainvalidator.h"


/** Constructor */
NetworkPage::NetworkPage(QWidget *parent)
: ConfigPage(parent, tr("Network"))
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
 
  connect(ui.btnAddBridge, SIGNAL(clicked()), this, SLOT(addBridge()));
  connect(ui.btnRemoveBridge, SIGNAL(clicked()), this, SLOT(removeBridge()));

  ui.lineHttpProxyAddress->setValidator(new DomainValidator(this));
  ui.lineHttpProxyPort->setValidator(new QIntValidator(1, 65535, this));
}

/** Destructor */
NetworkPage::~NetworkPage()
{
}

/** Adds a bridge to the bridge list box. */
void
NetworkPage::addBridge()
{
  QString bridge = ui.lineBridge->text().trimmed();
  if (bridge.isEmpty())
    return;
  if (!ui.listBridges->findItems(bridge, Qt::MatchFixedString).isEmpty())
    return; /* duplicate bridge */

  /* XXX: We should do some verification that the gibberish the user typed in
   * is actually valid. */
  ui.listBridges->addItem(bridge);
}

/** Removes one or more selected bridges from the bridge list box. */
void
NetworkPage::removeBridge()
{
  qDeleteAll(ui.listBridges->selectedItems());
}

/** Saves changes made to settings on the Firewall settings page. */
bool
NetworkPage::save(QString &errmsg)
{
  NetworkSettings settings;
  QStringList bridgeList;
  QList<quint16> reachablePorts;
  bool ok;
  
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
  foreach (QString portString, ui.lineReachablePorts->text().split(",")) {
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
  NetworkSettings settings;
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
  ui.chkUseBridges->setChecked(settings.getUseBridges());
  ui.listBridges->addItems(settings.getBridgeList());
}

