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
** \file NetworkSettings.cpp
** \version $Id$
** \brief Settings for configuring an HTTP/HTTPS proxy or bridges
*/

#include "NetworkSettings.h"

#define SETTING_FASCIST_FIREWALL    "FascistFirewall"
#define SETTING_REACHABLE_ADDRESSES "ReachableAddresses"

/* Vidalia-specific proxy options */
#define SETTING_PROXY_TYPE          "ProxyType"
#define SETTING_PROXY_ADDRESS       "ProxyAddress"
#define SETTING_PROXY_USERNAME      "ProxyUsername"
#define SETTING_PROXY_PASSWORD      "ProxyPassword"

/* Tor's proxy options */
#define SETTING_HTTP_PROXY          "HttpProxy"
#define SETTING_HTTP_PROXY_AUTH     "HttpProxyAuthenticator"
#define SETTING_HTTPS_PROXY         "HttpsProxy"
#define SETTING_HTTPS_PROXY_AUTH    "HttpsProxyAuthenticator"
#define SETTING_SOCKS4_PROXY        "Socks4Proxy"
#define SETTING_SOCKS5_PROXY        "Socks5Proxy"
#define SETTING_SOCKS5_USERNAME     "Socks5ProxyUsername"
#define SETTING_SOCKS5_PASSWORD     "Socks5ProxyPassword"

#define SETTING_USE_BRIDGES         "UseBridges"
#define SETTING_BRIDGE_LIST         "Bridge"
#define SETTING_UPDATE_BRIDGES      "UpdateBridgesFromAuthority"
#define SETTING_TUNNEL_DIR_CONNS    "TunnelDirConns"
#define SETTING_PREFER_TUNNELED_DIR_CONNS "PreferTunneledDirConns"


/** Default constructor */
NetworkSettings::NetworkSettings(TorControl *torControl)
: AbstractTorSettings("Network", torControl)
{
  setDefault(SETTING_PROXY_TYPE,        NoProxy);
  setDefault(SETTING_PROXY_ADDRESS,     "");
  setDefault(SETTING_PROXY_USERNAME,    "");
  setDefault(SETTING_PROXY_PASSWORD,    "");
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
 
  QString socks4, socks5, http, https;
  QString addr, user, pass, auth;

  addr = localValue(SETTING_PROXY_ADDRESS).toString();
  user = localValue(SETTING_PROXY_USERNAME).toString();
  pass = localValue(SETTING_PROXY_PASSWORD).toString();

  if (!user.isEmpty() || !pass.isEmpty())
    auth = QString("%1:%2").arg(user).arg(pass);
 
  switch (getProxyType()) {
    case NoProxy:
      break;
    case Socks4Proxy:
      socks4 = addr;
      break;
    case Socks5Proxy:
      socks5 = addr;
      break;
    case HttpProxy:
      http = addr;
      break;
    case HttpHttpsProxy:
      http = addr;
      https = http;
      break;
  }

  if (torVersion >= 0x020201) {
    /* SOCKS support was implemented in 0.2.2.1 */
    conf.insert(SETTING_SOCKS4_PROXY, socks4);
    conf.insert(SETTING_SOCKS5_PROXY, socks5);
    conf.insert(SETTING_SOCKS5_USERNAME, user);
    conf.insert(SETTING_SOCKS5_PASSWORD, pass);
  }

  conf.insert(SETTING_HTTP_PROXY, http);
  conf.insert(SETTING_HTTPS_PROXY, https);
  conf.insert(SETTING_HTTP_PROXY_AUTH, auth);
  conf.insert(SETTING_HTTPS_PROXY_AUTH, auth);

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

/** Returns the proxy type Tor is using, or NoProxy if it makes direct
 * connections. */ 
NetworkSettings::ProxyType
NetworkSettings::getProxyType()
{
  QString type = value(SETTING_PROXY_TYPE).toString();
  return proxyTypeFromString(type);
}

/** Set the type of proxy Tor should use to <b>type</b>. */
void
NetworkSettings::setProxyType(ProxyType type)
{
  setValue(SETTING_PROXY_TYPE, proxyTypeToString(type));
}

/** Returns the address of the proxy server Tor makes connections through. */
QString
NetworkSettings::getProxyAddress()
{
  return value(SETTING_PROXY_ADDRESS).toString();
}

/** Sets the proxy address and port to <b>addr</b>. */
void
NetworkSettings::setProxyAddress(const QString &addr)
{
  setValue(SETTING_PROXY_ADDRESS, addr);
}

/** Returns the username used to login to the proxy server. */
QString
NetworkSettings::getProxyUsername()
{
  return value(SETTING_PROXY_USERNAME).toString();
}

/** Sets the proxy server username to <b>user</b>. */ 
void
NetworkSettings::setProxyUsername(const QString &user)
{
  setValue(SETTING_PROXY_USERNAME, user);
}

/** Returns the password used to login to the proxy server. */
QString
NetworkSettings::getProxyPassword()
{
  return value(SETTING_PROXY_PASSWORD).toString();
}

/** Sets the proxy server password to <b>pass</b>. */ 
void
NetworkSettings::setProxyPassword(const QString &pass)
{
  setValue(SETTING_PROXY_PASSWORD, pass);
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

/** Converts the ProxyType <b>type</b> to a string to store in the
 * configuration file. */
QString
NetworkSettings::proxyTypeToString(ProxyType type)
{
  QString ret;

  switch (type) {
    case Socks4Proxy:
      ret = "socks4";
      break;
    case Socks5Proxy:
      ret = "socks5";
      break;
    case HttpProxy:
      ret = "http";
      break;
    case HttpHttpsProxy:
      ret = "httphttps";
      break;
    case NoProxy:
    default:
      ret = "none";
      break;
  }

  return ret;
}

/** Converts the proxy type string <b>type</b> to its ProxyType counterpart. */
NetworkSettings::ProxyType
NetworkSettings::proxyTypeFromString(const QString &type)
{
  QString str = type.toLower();
  
  if (str == "socks4")
    return Socks4Proxy;
  if (str == "socks5")
    return Socks5Proxy;
  if (str == "http")
    return HttpProxy;
  if (str == "httphttps")
    return HttpHttpsProxy;

  return NoProxy;
}

