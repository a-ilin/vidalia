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
** \file ServerSettings.h
** \brief Settings for running a Tor server
*/

#ifndef _SERVERSETTINGS_H
#define _SERVERSETTINGS_H

#include "AbstractTorSettings.h"
#include "ExitPolicy.h"


class ServerSettings : public AbstractTorSettings
{
  Q_OBJECT

public:
  /** Constructor */
  ServerSettings(TorControl *torControl);

  /** Applies changese to Tor. */
  bool apply(QString *errmsg = 0);

  /** Enables running Tor as a server. */
  void setServerEnabled(bool enable);
  /** Returns true if Tor is running as a server. */
  bool isServerEnabled();

  /** Sets to <b>enabled</b> whether Tor should be a bridge node when acting
   * as a server. */
  void setBridgeEnabled(bool enable);
  /** Returns true if Tor is configured to act as a bridge node. */
  bool isBridgeEnabled();

  /** Sets to <b>enabled</b> whether Tor should be a non-exit node when acting
   * as a server. */
  void setNonExitEnabled(bool enable);
  /** Returns true if Tor is configured to act as a non-exit node. */
  bool isNonExitEnabled();

  /** Sets the server's ORPort value. */
  void setORPort(quint16 orPort);
  /** Gets the server's ORPort value. */
  quint16 getORPort();

  /** Sets the server's DirPort value. */
  void setDirPort(quint16 dirPort);
  /** Gets the server's DirPort value. */
  quint16 getDirPort();

  /** Sets the server's nickname. */
  void setNickname(QString nickname);
  /** Gets the server's nickname. */
  QString getNickname();

  /** Sets the server operator's contact information. */
  void setContactInfo(QString info);
  /** Gets the server operator's contact information. */
  QString getContactInfo();

  /** Enables or disables the server to act as a directory mirror. */
  void setDirectoryMirror(bool mirror);
  /** Returns true if the server will mirror the directory. */
  bool isDirectoryMirror();
  /** Sets the exit policy for this server. */
  void setExitPolicy(ExitPolicy &policy);
  /** Gets the exit policy for this server. */
  ExitPolicy getExitPolicy();

  /** Sets the long-term average bandwidth rate (in KB/s) of this server. */
  void setBandwidthAvgRate(quint32 rate);
  /** Gets the long-term average bandwidth rate (in KB/s) of this server. */
  quint32 getBandwidthAvgRate();

  /** Sets the maximum burst rate (in B/s) of this server. */
  void setBandwidthBurstRate(quint32 rate);
  /** Gets the maximum burst rate (in B/s) of this server. */
  quint32 getBandwidthBurstRate();

  /** Sets whether the user's server descriptor will be published or not.
   * Currently this only affects publishing of bridge descriptors. If the
   * user is running a normal relay, its descriptor will always be
   * published regardless of this setting. */
  void setPublishServerDescriptor(bool publish);
  /** Returns true if the user's server descriptor will be published to
   * the appropriate authorities. */
  bool publishServerDescriptor();

  /** Configure port forwarding. */
  void configurePortForwarding();

  void cleanupPortForwarding();

  /** Returns true if UPnP support is enabled. */
  bool isUpnpEnabled();
  /** Sets whether Vidalia should try to configure port forwarding using UPnP. */
  void setUpnpEnabled(bool enabled);

  /** Returns true if accounting is enabled */
  bool isAccountingEnabled();
  /** Disables accounting options */
  void disableAccounting();
  /** Sets AccountingMax to amount unit */
  void setAccountingMax(int amount, const QString &unit);
  /** Sets AccountingStart for dwm (day/week/month) day and time */
  void setAccountingStart(const QString &dwm, int day, const QString &time);

  /** Returns the max amount of bytes/KB/etc for accounting */
  int accountingMaxAmount();
  /** Returns the unit for the max amount */
  const QString accountingMaxUnit();
  /** Returns the configured AccountingStart (day/week/month) */
  const QString accountingStartDwm();
  /** Returns the day of the week/month for AccountingStart */
  int accountingStartDay();
  /** Returns the time of day for AccountingStart */
  const QString accountingStartTime();

protected:
  /** Virtual method called when we retrieve a server-related setting from Tor.
   * Currently this just translates BandwidthFoo to RelayBandwidthFoo when
   * appropriate. */
  virtual QVariant torValue(const QString &key) const;
};

#endif

