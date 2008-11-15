/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file networksettings.cpp
** \version $Id$
** \brief Settings for configuring an HTTP/HTTPS proxy or bridges
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
#define SETTING_TUNNEL_DIR_CONNS    "TunnelDirConns"
#define SETTING_PREFER_TUNNELED_DIR_CONNS "PreferTunneledDirConns"


/** Default constructor */
NetworkSettings::NetworkSettings(TorControl *torControl)
: AbstractTorSettings("Network", torControl)
{
  setDefault(SETTING_USE_HTTP_PROXY,    false);
  setDefault(SETTING_HTTP_PROXY,        "");
  setDefault(SETTING_HTTP_PROXY_AUTH,   "");
  setDefault(SETTING_USE_HTTPS_PROXY,   true);
  setDefault(SETTING_HTTPS_PROXY,       "");
  setDefault(SETTING_HTTPS_PROXY_AUTH,  "");
  setDefault(SETTING_USE_BRIDGES,       false);
  setDefault(SETTING_BRIDGE_LIST,       QStringList());
  setDefault(SETTING_FASCIST_FIREWALL,  false);
  setDefault(SETTING_TUNNEL_DIR_CONNS,  true);
  setDefault(SETTING_REACHABLE_ADDRESSES,
    QStringList() << "*:80" << "*:443");
}

/** Applies the current network configuration settings to Tor. If
 * <b>errmsg</b> is specified and an error occurs while applying the settings,
 * it will be set to a string describing the error. */
bool
NetworkSettings::apply(QString *errmsg)
{
  QMultiHash<QString, QString> conf;
  quint32 torVersion = torControl()->getTorVersion();

  conf.insert(SETTING_REACHABLE_ADDRESSES,
    (getFascistFirewall() ? 
      localValue(SETTING_REACHABLE_ADDRESSES).toStringList().join(",") : ""));

  if (getUseHttpProxy())
    conf.insert(SETTING_HTTP_PROXY, localValue(SETTING_HTTP_PROXY).toString());
  else
    conf.insert(SETTING_HTTP_PROXY,  "");
  conf.insert(SETTING_HTTP_PROXY_AUTH,
              localValue(SETTING_HTTP_PROXY_AUTH).toString());

  if (getUseHttpProxy() && getUseHttpsProxy())
    conf.insert(SETTING_HTTPS_PROXY, localValue(SETTING_HTTPS_PROXY).toString());
  else
    conf.insert(SETTING_HTTPS_PROXY, "");
  conf.insert(SETTING_HTTPS_PROXY_AUTH,
              localValue(SETTING_HTTPS_PROXY_AUTH).toString());

  if (getUseBridges()) {
    /* We want to always enable TunnelDirConns and friends when using
     * bridge relays. */
    conf.insert(SETTING_TUNNEL_DIR_CONNS, "1");
    conf.insert(SETTING_PREFER_TUNNELED_DIR_CONNS, "1");
  } else if (torVersion <= 0x020021) {
    /* TunnelDirConns is enabled by default on Tor >= 0.2.0.22-rc, so don't
     * disable it if our Tor is recent enough. */
    conf.insert(SETTING_TUNNEL_DIR_CONNS, "0");
    conf.insert(SETTING_PREFER_TUNNELED_DIR_CONNS, "0");
  }

  if (torVersion >= 0x020003) {
    /* Do the bridge stuff only on Tor >= 0.2.0.3-alpha */
    QStringList bridges = localValue(SETTING_BRIDGE_LIST).toStringList();
    if (getUseBridges() && !bridges.isEmpty()) {
      conf.insert(SETTING_USE_BRIDGES, "1");
      conf.insert(SETTING_UPDATE_BRIDGES, "1");
      foreach (QString bridge, bridges) {
        conf.insert(SETTING_BRIDGE_LIST, bridge);
      }
    } else {
      conf.insert(SETTING_USE_BRIDGES, "0");
      conf.insert(SETTING_BRIDGE_LIST, "");
      conf.insert(SETTING_UPDATE_BRIDGES, "0");
    }
  }
  return torControl()->setConf(conf, errmsg);
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
  QStringList lineList;
  bool ok;

  lineList = value(SETTING_REACHABLE_ADDRESSES).toStringList();
  foreach (QString line, lineList) {
    foreach (QString address, line.split(",", QString::SkipEmptyParts)) {
      QStringList parts = address.split(":");
      if (parts.size() >= 2) {
        quint16 port = parts.at(1).toUInt(&ok);
        if (ok)
          reachablePorts << port;
      }
    }
  }
  return reachablePorts;
}

/** Sets the list of ports that will be specified in ReachableAddresses to
 * <b>reachablePorts</b>. */
void
NetworkSettings::setReachablePorts(const QList<quint16> &reachablePorts)
{
  if (!reachablePorts.isEmpty()) {
    QStringList portList;
    foreach (quint16 port, reachablePorts) {
      portList << "*:" + QString::number(port);
    }
    setValue(SETTING_REACHABLE_ADDRESSES, portList);
  }
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

/** Returns true if Tor is configured to try to tunnel its directory
 * connections through a one-hop circuit. */
bool
NetworkSettings::getTunnelDirConns()
{
  return value(SETTING_TUNNEL_DIR_CONNS).toBool();
}

