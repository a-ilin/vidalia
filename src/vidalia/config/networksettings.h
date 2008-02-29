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
** \file networksettings.h
** \version $Id$
** \brief Settings for configuring an HTTP/HTTPS proxy or bridges
*/

#ifndef _NETWORKSETTINGS_H
#define _NETWORKSETTINGS_H

#include <QList>
#include <QStringList>

#include "abstracttorsettings.h"


class NetworkSettings : public AbstractTorSettings
{
  Q_OBJECT

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

  /** Returns true if Tor should make all its directory requests through a
   * proxy. */
  bool getUseHttpProxy();
  /** Sets to <b>useHttpProxy</b> whether Tor should make all its directory
   * requests through the proxy specified to setHttpProxy()
   * \sa setHttpProxy() */
  void setUseHttpProxy(bool useHttpProxy);
  
  /** Returns the proxy used for making Tor's directory requests, in the form
   * of <i>host[:port]</i>. */
  QString getHttpProxy();
  /** Sets the proxy used for making Tor's directory requests. <b>proxy</b>
   * should be in the form <i>host[:port]</i>. If <i>:port</i> is not
   * specified, then Tor will use its default of port 80. */
  void setHttpProxy(const QString &proxy);

  /** Returns the authentication information Tor should use to authenticate to
   * an Http proxy. The returned value is in the form 
   * <i>username:password</i>. */
  QString getHttpProxyAuthenticator();
  /** Sets the authentication information required by an Http proxy.
   * <b>authenticator</b> should be in the form <i>username:password</i>. */
  void setHttpProxyAuthenticator(const QString &authenticator);

  /** Returns true if Tor should make all its OR connections through a
   * proxy. */
  bool getUseHttpsProxy();
  /** Sets to <b>useHttpsProxy</b> whether Tor should make all its OR
   * connections through the proxy specified to setHttpsProxy.
   * \sa setHttpsProxy() */
  void setUseHttpsProxy(bool useHttpsProxy);
  
  /** Returns the proxy used for making Tor's OR connections, in the form
   * of <i>host[:port]</i>. */
  QString getHttpsProxy();
  /** Sets the proxy used for making Tor's OR connections. <b>proxy</b>
   * should be in the form <i>host[:port]</i>. If <i>:port</i> is not
   * specified, then Tor will use its default of port 443. */
  void setHttpsProxy(const QString &proxy);

  /** Returns the authentication information Tor should use to authenticate to
   * an Https proxy. The returned value is in the form 
   * <i>username:password</i>. */
  QString getHttpsProxyAuthenticator();
  /** Sets the authentication information required by an Https proxy.
   * <b>authenticator</b> should be in the form <i>username:password</i>. */
  void setHttpsProxyAuthenticator(const QString &authenticator);
 
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
};

#endif

