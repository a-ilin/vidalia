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
** \file NetworkSettings.h
** \brief Settings for configuring an HTTP/HTTPS proxy or bridges
*/

#ifndef _NETWORKSETTINGS_H
#define _NETWORKSETTINGS_H

#include "AbstractTorSettings.h"

#include <QList>
#include <QStringList>


class NetworkSettings : public AbstractTorSettings
{
  Q_OBJECT

public:
  enum ProxyType
  {
    ProxyTypeMin = -1,
    NoProxy = -1, /**< Have Tor connect directly to the Internet. */
    Socks4Proxy = 0, /**< Use a SOCKS 4 proxy for OR connections. */
    Socks5Proxy = 1, /**< Use a SOCKS 5 proxy for OR connections. */
    HttpHttpsProxy = 2, /**< Use HTTP proxy for both dir and OR connections. */
    ProxyTypeMax = 2
  };

public:
  /** Default constructor. */
  NetworkSettings(TorControl *torControl);

  /** Applies the current network configuration settings to Tor. If
   *  * <b>errmsg</b> is specified and an error occurs while applying the
   *  settings, it will be set to a string describing the error. */
  bool apply(QString *errmsg = 0);

  /** Returns true if we need to set ReachableAddresses because we're behind a
   * restrictive firewall that limits the ports Tor can connect to. */
  bool getFascistFirewall();
  /** Sets to <b>fascistFirewall</b> whether Tor should only create outgoing
   * connections to the list of ports specified to setReachablePorts().
   * \sa setReachablePorts() */
  void setFascistFirewall(bool fascistFirewall);

  /** Returns a list of ports to be specified in ReachableAddresses. */
  QList<quint16> getReachablePorts();
  /** Sets the list of ports that will be specified in ReachableAddresses to
   * <b>reachablePorts</b>. */
  void setReachablePorts(const QList<quint16> &reachablePorts);

  /** Returns the proxy type Tor is using, or NoProxy if it makes direct
   * connections. */
  ProxyType getProxyType();
  /** Set the type of proxy Tor should use to <b>type</b>. */
  void setProxyType(ProxyType type);

  /** Returns the address of the proxy server Tor makes connections through. */
  QString getProxyAddress();
  /** Sets the proxy address and port to <b>addr</b>. */
  void setProxyAddress(const QString &addr);

  /** Returns the username used to login to the proxy server. */
  QString getProxyUsername();
  /** Sets the proxy server username to <b>user</b>. */
  void setProxyUsername(const QString &user);

  /** Returns the password used to login to the proxy server. */
  QString getProxyPassword();
  /** Sets the proxy server password to <b>pass</b>. */
  void setProxyPassword(const QString &pass);

  /** Returns true if Tor should try to use bridge nodes to access the Tor
   * network. */
  bool getUseBridges();
  /** Sets to <b>useBridges</b> whether Tor should try to use bridge nodes
   * to access the Tor network. */
  void setUseBridges(bool useBridges);

  /** Returns a list of bridge nodes Tor should use. */
  QStringList getBridgeList();
  /** Sets to <b>bridgeList</b> the list of bridge nodes Tor should use. */
  void setBridgeList(const QStringList &bridgeList);

  /** Returns true if Tor is configured to try to tunnel its directory
   * connections through a one-hop circuit. */
  bool getTunnelDirConns();

private:
  /** Converts the ProxyType <b>type</b> to a string to store in the
   * configuration file. */
  QString proxyTypeToString(ProxyType type);

  /** Converts the proxy type string <b>type</b> to its ProxyType counterpart. */
  ProxyType proxyTypeFromString(const QString &type);
};

#endif

