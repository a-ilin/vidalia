/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
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
 * \file networksettings.cpp
 * \version $Id$
 * \brief Settings for configuring an HTTP/HTTPS proxy or bridges
 */

#include "networksettings.h"

#define SETTING_FASCIST_FIREWALL    "FascistFirewall"
#define SETTING_REACHABLE_ADDRESSES "ReachableAddresses"
#define SETTING_USE_HTTP_PROXY      "UseHttpProxy"
#define SETTING_HTTP_PROXY          "HttpProxy"
#define SETTING_HTTP_PROXY_AUTH     "HttpProxyAuthenticator"
#define SETTING_USE_HTTPS_PROXY     "UseHttpsProxy"
#define SETTING_HTTPS_PROXY         "HttpsProxy"
#define SETTING_HTTPS_PROXY_AUTH    "HttpsProxyAuthenticator"
#define SETTING_USE_BRIDGES         "UseBridges"
#define SETTING_BRIDGE_LIST         "Bridge"
#define SETTING_UPDATE_BRIDGES      "UpdateBridgesFromAuthority"


/** Default constructor */
NetworkSettings::NetworkSettings(TorControl *torControl)
: AbstractTorSettings("Network", torControl)
{
  setDefault(SETTING_USE_HTTP_PROXY,    false);
  setDefault(SETTING_HTTP_PROXY,        "");
  setDefault(SETTING_HTTP_PROXY_AUTH,   "");
  setDefault(SETTING_USE_HTTPS_PROXY,   false);
  setDefault(SETTING_HTTPS_PROXY,       "");
  setDefault(SETTING_HTTPS_PROXY_AUTH,  "");
  setDefault(SETTING_USE_BRIDGES,       false);
  setDefault(SETTING_BRIDGE_LIST,       QStringList());
  setDefault(SETTING_FASCIST_FIREWALL,  false);
  setDefault(SETTING_REACHABLE_ADDRESSES, QStringList());
}

/** Applies the current network configuration settings to Tor. If
 * <b>errmsg</b> is specified and an error occurs while applying the settings,
 * it will be set to a string describing the error. */
bool
NetworkSettings::apply(QString *errmsg)
{
  QMultiHash<QString, QString> conf;
  
  conf.insert(SETTING_REACHABLE_ADDRESSES,
    (getFascistFirewall() ? 
      localValue(SETTING_REACHABLE_ADDRESSES).toStringList().join(",") : ""));
  
  conf.insert(SETTING_HTTP_PROXY,
    (getUseHttpProxy() ? localValue(SETTING_HTTP_PROXY).toString() : ""));
  conf.insert(SETTING_HTTP_PROXY_AUTH,
              localValue(SETTING_HTTP_PROXY_AUTH).toString());
  
  conf.insert(SETTING_HTTPS_PROXY,
    (getUseHttpsProxy() ? localValue(SETTING_HTTPS_PROXY).toString() : ""));
  conf.insert(SETTING_HTTPS_PROXY_AUTH,
              localValue(SETTING_HTTPS_PROXY_AUTH).toString());
  
  if (getUseBridges()) {
    conf.insert(SETTING_USE_BRIDGES, "1");
    conf.insert(SETTING_UPDATE_BRIDGES, "1");
    foreach (QString bridge, localValue(SETTING_BRIDGE_LIST).toStringList()) {
      conf.insert(SETTING_BRIDGE_LIST, bridge);
    }
  } else {
    conf.insert(SETTING_USE_BRIDGES, "0");
    conf.insert(SETTING_BRIDGE_LIST, "");
    conf.insert(SETTING_UPDATE_BRIDGES, "0");
  }

  return _torControl->setConf(conf, errmsg);
}

/** Returns true if we need to set ReachableAddresses because we're behind a
 * restrictive firewall that limits the ports Tor can connect to. */
bool
NetworkSettings::getFascistFirewall()
{
  return localValue(SETTING_FASCIST_FIREWALL).toBool();
}

/** Sets to <b>fascistFirewall</b> whether Tor should only create outgoing
 * connections to the list of ports specified in setReachablePorts().
 * \sa setReachablePorts() */
void
NetworkSettings::setFascistFirewall(bool fascistFirewall)
{
  setValue(SETTING_FASCIST_FIREWALL, fascistFirewall);
}

/** Returns a list of ports to be specified in ReachableAddresses. */
QList<quint16>
NetworkSettings::getReachablePorts()
{
  QList<quint16> reachablePorts;
  QStringList addressList;
  bool ok;

  addressList = value(SETTING_REACHABLE_ADDRESSES).toStringList();
  foreach(QString address, addressList) {
    QStringList parts = address.split(":");
    if (parts.size() >= 2) {
      quint16 port = parts.at(1).toUInt(&ok);
      if (ok)
        reachablePorts << port;
    }
  }
  return reachablePorts;
}

/** Sets the list of ports that will be specified in ReachableAddresses to
 * <b>reachablePorts</b>. */
void
NetworkSettings::setReachablePorts(const QList<quint16> &reachablePorts)
{
  QStringList portList;
  foreach (quint16 port, reachablePorts) {
    portList << "*:" + QString::number(port);
  }
  setValue(SETTING_REACHABLE_ADDRESSES, portList);
}

/** Returns true if Tor should make all its directory requests through a
 * proxy. */
bool
NetworkSettings::getUseHttpProxy()
{
  return localValue(SETTING_USE_HTTP_PROXY).toBool();
}

/** Sets to <b>useHttpProxy</b> whether Tor should make all its directory
 * requests through the proxy specified to setHttpProxy().
 * \sa setHttpProxy() */
void
NetworkSettings::setUseHttpProxy(bool useHttpProxy)
{
  setValue(SETTING_USE_HTTP_PROXY, useHttpProxy);
}

/** Returns the proxy used for making Tor's directory requests, in the form
 * of <i>host[:port]</i>. */
QString
NetworkSettings::getHttpProxy()
{
  return value(SETTING_HTTP_PROXY).toString();
}

/** Sets the proxy used for making Tor's directory requests. <b>proxy</b>
 * should be in the form <i>host[:port]</i>. If <i>:port</i> is not
 * specified, then Tor will use its default of port 80. */
void
NetworkSettings::setHttpProxy(const QString &proxy)
{
  setValue(SETTING_HTTP_PROXY, proxy);
}

/** Returns the authentication information Tor should use to authenticate to
 * an Http proxy. The returned value is in the form 
 * <i>username:password</i>. */
QString
NetworkSettings::getHttpProxyAuthenticator()
{
  return value(SETTING_HTTP_PROXY_AUTH).toString();
}

/** Sets the authentication information required by an Http proxy.
 * <b>authenticator</b> should be in the form <i>username:password</i>. */
void
NetworkSettings::setHttpProxyAuthenticator(const QString &auth)
{
  setValue(SETTING_HTTP_PROXY_AUTH, auth);
}

/** Returns true if Tor should make all its OR connections through a
 * proxy. */
bool
NetworkSettings::getUseHttpsProxy()
{
  return localValue(SETTING_USE_HTTPS_PROXY).toBool();
}

/** Sets to <b>useHttpsProxy</b> whether Tor should make all its OR
 * connections through the proxy specified to setHttpsProxy().
 * \sa setHttpsProxy() */
void
NetworkSettings::setUseHttpsProxy(bool useHttpsProxy)
{
  setValue(SETTING_USE_HTTPS_PROXY, useHttpsProxy);
}

/** Returns the proxy used for making Tor's OR connections, in the form
 * of <i>host[:port]</i>. */
QString
NetworkSettings::getHttpsProxy()
{
  return value(SETTING_HTTPS_PROXY).toString();
}

/** Sets the proxy used for making Tor's OR connections. <b>proxy</b>
 * should be in the form <i>host[:port]</i>. If <i>:port</i> is not
 * specified, then Tor will use its default of port 443. */
void
NetworkSettings::setHttpsProxy(const QString &proxy)
{
  setValue(SETTING_HTTPS_PROXY, proxy);
}

/** Returns the authentication information Tor should use to authenticate to
 * an Https proxy. The returned value is in the form 
 * <i>username:password</i>. */
QString
NetworkSettings::getHttpsProxyAuthenticator()
{
  return value(SETTING_HTTPS_PROXY_AUTH).toString();
}

/** Sets the authentication information required by an Https proxy.
 * <b>authenticator</b> should be in the form <i>username:password</i>. */
void
NetworkSettings::setHttpsProxyAuthenticator(const QString &auth)
{
  setValue(SETTING_HTTPS_PROXY_AUTH, auth);
}

/** Returns true if Tor should try to use bridge nodes to access the Tor
 * network. */
bool
NetworkSettings::getUseBridges()
{
  return value(SETTING_USE_BRIDGES).toBool();
}

/** Sets to <b>useBridges</b> whether Tor should try to use bridge nodes
 * to access the Tor network. */
void
NetworkSettings::setUseBridges(bool useBridges)
{
  setValue(SETTING_USE_BRIDGES, useBridges);
}

/** Returns a list of bridge nodes Tor should use. */
QStringList
NetworkSettings::getBridgeList()
{
  return value(SETTING_BRIDGE_LIST).toStringList();
}

/** Sets to <b>bridgeList</b> the list of bridge nodes Tor should use. */
void
NetworkSettings::setBridgeList(const QStringList &bridgeList)
{
  setValue(SETTING_BRIDGE_LIST, bridgeList);
}

