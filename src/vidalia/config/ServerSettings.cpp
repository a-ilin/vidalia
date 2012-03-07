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
** \file ServerSettings.cpp
** \brief Settings for running a Tor server
*/

#include "config.h"
#include "ServerSettings.h"
#include "TorSettings.h"
#include "TorControl.h"
#ifdef USE_MINIUPNPC
#include "UPNPControl.h"
#endif
#include "Vidalia.h"

#include "net.h"
#include "stringutil.h"

#include <QHostInfo>

/** Define the set of characters that are valid in a nickname. */
#define VALID_NICKNAME_CHARS \
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
/** Define the maximum length of a server's nickname. */
#define MAX_NICKNAME_LEN   19

/* Server configuration settings */
#define SETTING_ENABLED         "Enabled"
#define SETTING_DIRMIRROR       "DirectoryMirror"
#define SETTING_NICKNAME        "Nickname"
#define SETTING_ORPORT          "ORPort"
#define SETTING_DIRPORT         "DirPort"
#define SETTING_CONTACT         "ContactInfo"
#define SETTING_EXITPOLICY      "ExitPolicy"
#define SETTING_BANDWIDTH_RATE  "BandwidthRate"
#define SETTING_BANDWIDTH_BURST "BandwidthBurst"
#define SETTING_BRIDGE_RELAY    "BridgeRelay"
#define SETTING_NONEXIT_RELAY   "NonExitRelay"
#define SETTING_ENABLE_UPNP     "EnableUPnP"
#define SETTING_RELAY_BANDWIDTH_RATE   "RelayBandwidthRate"
#define SETTING_RELAY_BANDWIDTH_BURST  "RelayBandwidthBurst"
#define SETTING_PUBLISH_SERVER_DESCRIPTOR "PublishServerDescriptor"
#define SETTING_ACCOUNTING_MAX "AccountingMax"
#define SETTING_ACCOUNTING_START "AccountingStart"

/** Constructor.
 * \param torControl a TorControl object used to read and apply the server
 * configuration settings.
 */
ServerSettings::ServerSettings(TorControl *torControl)
: AbstractTorSettings("Server", torControl)
{
  setDefault(SETTING_ENABLED,       false);
  setDefault(SETTING_DIRMIRROR,     true);
#if defined(Q_OS_WIN32)
  setDefault(SETTING_ORPORT,        443);
#else
  setDefault(SETTING_ORPORT,        9001);
#endif
  setDefault(SETTING_DIRPORT,       9030);
  setDefault(SETTING_NICKNAME,      "Unnamed");
  setDefault(SETTING_CONTACT,       "<you@example.com>");
  setDefault(SETTING_BANDWIDTH_RATE,        5242880);
  setDefault(SETTING_RELAY_BANDWIDTH_RATE,  5242880);
  setDefault(SETTING_BANDWIDTH_BURST,       10485760);
  setDefault(SETTING_RELAY_BANDWIDTH_BURST, 10485760);
  setDefault(SETTING_EXITPOLICY,
    ExitPolicy(ExitPolicy::Middleman).toString());
  setDefault(SETTING_ENABLE_UPNP, false); 
  setDefault(SETTING_BRIDGE_RELAY, false);
  setDefault(SETTING_PUBLISH_SERVER_DESCRIPTOR, "1");
}

/** Applies the current server configuration settings to Tor. If <b>errmsg</b>
 * is specified and an error occurs while applying the settings, it will be 
 * set to a string describing the error. */
bool
ServerSettings::apply(QString *errmsg)
{
  Torrc *torrc = Vidalia::torrc();

  configurePortForwarding();

  if (volatileValue(SETTING_ENABLED).toBool()) {
    quint32 torVersion = torControl()->getTorVersion();

    /* Server Nickname */
    torrc->setValue(SETTING_NICKNAME,
                volatileValue(SETTING_NICKNAME).toString());
    /* Server ORPort */
    torrc->setValue(SETTING_ORPORT,
                volatileValue(SETTING_ORPORT).toString());
    /* Server DirPort */
    torrc->setValue(SETTING_DIRPORT, 
                volatileValue(SETTING_DIRPORT).toString());
    /* Server Exit Policy */
    torrc->clear(QStringList() << SETTING_EXITPOLICY);
    if(volatileValue(SETTING_BRIDGE_RELAY).toBool() || 
       volatileValue(SETTING_NONEXIT_RELAY).toBool()) {
      torrc->setValue(SETTING_EXITPOLICY, "reject *:*");
    } else {
      torrc->setValue(SETTING_EXITPOLICY, volatileValue(SETTING_EXITPOLICY).toString());
    }
    /* Server bandwidth settings */
    torrc->setValue((torVersion >= 0x020001 ? SETTING_RELAY_BANDWIDTH_RATE 
                 : SETTING_BANDWIDTH_RATE),
                QString::number(volatileValue(SETTING_BANDWIDTH_RATE).toUInt()) + " bytes");
    torrc->setValue((torVersion >= 0x020001 ? SETTING_RELAY_BANDWIDTH_BURST
                 : SETTING_BANDWIDTH_BURST),
                QString::number(volatileValue(SETTING_BANDWIDTH_BURST).toUInt()) + " bytes");
    
    /* Server Contact Information */
    QString contact = 
      volatileValue(SETTING_CONTACT).toString().trimmed();
    QString defaultContact = defaultValue(SETTING_CONTACT).toString();
    if ((contact == defaultContact) ||
        (contact == scrub_email_addr(defaultContact))) {
      /* Only set the contact info if they put something non-default there */
      contact = "";
    }
    torrc->setValue(SETTING_CONTACT, scrub_email_addr(contact));
  
    /* Set if we're a bridge relay */
    if (volatileValue(SETTING_BRIDGE_RELAY).toBool()) {
      torrc->setValue(SETTING_BRIDGE_RELAY, "1");
      torrc->setValue(SETTING_PUBLISH_SERVER_DESCRIPTOR,
                  publishServerDescriptor() ? "1" : "0");
    } else {
      torrc->setValue(SETTING_BRIDGE_RELAY, "0");
      torrc->setValue(SETTING_PUBLISH_SERVER_DESCRIPTOR, "1");
    }

    torrc->setValue(SETTING_ACCOUNTING_MAX, volatileValue(SETTING_ACCOUNTING_MAX).toString());
    torrc->setValue(SETTING_ACCOUNTING_START, volatileValue(SETTING_ACCOUNTING_START).toString());
  } else {
    QStringList resetKeys;
    quint32 torVersion = torControl()->getTorVersion();
    resetKeys << SETTING_ORPORT 
              << SETTING_NICKNAME 
              << SETTING_DIRPORT
              << SETTING_CONTACT
              << SETTING_EXITPOLICY
              << SETTING_BRIDGE_RELAY
              << SETTING_PUBLISH_SERVER_DESCRIPTOR;
    if (torVersion >= 0x020001) {
      resetKeys << SETTING_RELAY_BANDWIDTH_RATE
                << SETTING_RELAY_BANDWIDTH_BURST;
    } else {
      resetKeys << SETTING_BANDWIDTH_RATE
                << SETTING_BANDWIDTH_BURST;
    }
    torrc->clear(resetKeys);
  }
  return torrc->apply(torControl(), errmsg);
}

/* TODO: We should call this periodically, in case the router gets rebooted or forgets its UPnP settings */
/* TODO: Remove port forwarding when Tor is shutdown or the ORPort changes */
/* TODO: init_upnp() will block for up to 2 seconds. We should fire off a thread */

/** Configure UPnP device to forward DirPort and ORPort. If enable is
 * true, will forward ORPort and DirPort; otherwise will remove exising
 * port mappings */
void
ServerSettings::configurePortForwarding()
{
#ifdef USE_MINIUPNPC
  quint16 ORPort, DirPort;

  // This is how the tickbox should control UPNP
  if (!isUpnpEnabled())
    return;

  ORPort = getORPort();
  if (!isServerEnabled())
    ORPort = 0;

  DirPort = getDirPort();
  if (!isServerEnabled() || !isDirectoryMirror())
    DirPort = 0;

  UPNPControl *control = UPNPControl::instance();
  control->setDesiredState(DirPort, ORPort);
#endif
}

void
ServerSettings::cleanupPortForwarding()
{
#ifdef USE_MINIUPNPC
  UPNPControl::cleanup();
#endif
}

/** Virtual method called when we retrieve a server-related setting from Tor.
 * Currently this just translates BandwidthFoo to RelayBandwidthFoo when
 * appropriate. */
QVariant
ServerSettings::torValue(const QString &key) const
{
  if (torControl()->getTorVersion() >= 0x020001) {
    if (key == SETTING_BANDWIDTH_RATE)
      return AbstractTorSettings::torValue(SETTING_RELAY_BANDWIDTH_RATE);
    else if (key == SETTING_BANDWIDTH_BURST)
      return AbstractTorSettings::torValue(SETTING_RELAY_BANDWIDTH_BURST);
  }
  return AbstractTorSettings::torValue(key);
}

/** Enables or disables running Tor as a server. 
 * \param enable Whether to enable or disable the Tor server. 
 */
void
ServerSettings::setServerEnabled(bool enable)
{
  setVolatileValue(SETTING_ENABLED, enable);
}

/** Returns true if Tor is currently configured to run as a Tor server. If Tor
 * is running, we will check whether it has an ORPort defined. Otherwise, we
 * will use our saved settings. */
bool
ServerSettings::isServerEnabled()
{
  QString orPort;
  if (torControl()->isConnected() && !changedSinceLastApply()) {
    if (torControl()->getConf(SETTING_ORPORT, orPort))
      return (orPort.toUInt() > 0);
  }

  with_torrc_value(SETTING_ORPORT) {
    return ret.at(0).toUInt() > 0;
  }
  return false;
}

/** Sets to <b>enabled</b> whether Tor should be a bridge node when acting as
 * a server. */
void
ServerSettings::setBridgeEnabled(bool enabled)
{
  setVolatileValue(SETTING_BRIDGE_RELAY, enabled);
}

/** Returns true if Tor is configured to act as a bridge node. */
bool
ServerSettings::isBridgeEnabled()
{
  with_torrc_value(SETTING_BRIDGE_RELAY) {
    return (ret.at(0) == "1") && isServerEnabled();
  }
  return false;
}

/** Sets to <b>enabled</b> whether Tor should be a non-exit node when acting as
 * a server. */
void
ServerSettings::setNonExitEnabled(bool enabled)
{
  setVolatileValue(SETTING_NONEXIT_RELAY, enabled);
}

/** Returns true if Tor is configured to act as a non-exit node. */
bool
ServerSettings::isNonExitEnabled()
{
  with_torrc_value(SETTING_EXITPOLICY) {
    return ret.size() == 1 and ret.at(0) == "reject *:*" and isServerEnabled();
  }
  return false;
}

/** Sets the server's ORPort. */
void
ServerSettings::setORPort(quint16 orPort)
{
  setVolatileValue(SETTING_ORPORT, orPort);
}

/** Gets the server's current ORPort setting. */
quint16
ServerSettings::getORPort()
{
  quint16 val = 0;
  with_torrc_value(SETTING_ORPORT) {
    val = (quint16)ret.at(0).toUInt();
  } else {
    val = (quint16)defaultValue(SETTING_ORPORT).toUInt();
  }

  return val;
}

/** Sets the server's current DirPort. */
void
ServerSettings::setDirPort(quint16 dirPort)
{
  setVolatileValue(SETTING_DIRPORT, dirPort);
}

/** Gets the server's current DirPort. */
quint16
ServerSettings::getDirPort()
{
  quint16 val = 0;
  with_torrc_value(SETTING_DIRPORT) {
    val = (quint16)ret.at(0).toUInt();
  } else {
    val = (quint16)defaultValue(SETTING_DIRPORT).toUInt();
  }
  return val;
}

/** Sets the server's nickname. */
void
ServerSettings::setNickname(QString nickname)
{
  setVolatileValue(SETTING_NICKNAME, nickname);
}

/** Gets the server's nickname. */
QString
ServerSettings::getNickname()
{
  QString nickname = "<Unnamed>";
  with_torrc_value(SETTING_NICKNAME) {
    nickname = ret.at(0);
  } else {
    nickname = defaultValue(SETTING_NICKNAME).toString();
  }
  /* Ensure the nickname contains only valid characters and is not too long. */
  return ensure_valid_chars(nickname, 
                            VALID_NICKNAME_CHARS).left(MAX_NICKNAME_LEN);
}

/** Sets the server's contact information. */
void
ServerSettings::setContactInfo(QString contact)
{
  setVolatileValue(SETTING_CONTACT, contact);
}

/** Gets the server's contact information. */
QString
ServerSettings::getContactInfo()
{
  QString contact;
  with_torrc_value(SETTING_CONTACT) {
    contact = ret.at(0);
  } else {
    contact = defaultValue(SETTING_CONTACT).toString();
  }
  return contact;
}

/** Returns whether this server will act as a directory mirror or not. */
bool
ServerSettings::isDirectoryMirror()
{
  return getDirPort() > 0;
}

/** Sets whether this server will act as a directory mirror. */
void
ServerSettings::setDirectoryMirror(bool mirror)
{
  setVolatileValue(SETTING_DIRMIRROR, mirror);
}

/** Returns the exit policy for this server. */
ExitPolicy
ServerSettings::getExitPolicy()
{
  QString val = "";
  with_torrc_value(SETTING_EXITPOLICY) {
    foreach(QString ep, ret) {
      val = ep + "," + val;
    }
  } else {
    val = defaultValue(SETTING_EXITPOLICY).toString();
  }
  return ExitPolicy(val);
}

/** Sets the exit policy for this server. */
void
ServerSettings::setExitPolicy(ExitPolicy &exitPolicy)
{
  setVolatileValue(SETTING_EXITPOLICY, exitPolicy.toString());
}

/** Returns the long-term average bandwidth rate (in KB/s) for this server. */
quint32
ServerSettings::getBandwidthAvgRate()
{
  quint32 val = 0;
  with_torrc_value(SETTING_BANDWIDTH_RATE) {
    val = ret.at(0).trimmed().split(" ").at(0).toUInt();
  } else {
    val = defaultValue(SETTING_BANDWIDTH_RATE).toUInt();
  }

  return val;
}

/** Sets the long-term average bandwidth rate (in KB/s) for this server. */
void
ServerSettings::setBandwidthAvgRate(quint32 rate)
{
  setVolatileValue(SETTING_BANDWIDTH_RATE, rate);
}

/** Returns the maximum bandwidth burst rate (in KB/s) for this server. */
quint32
ServerSettings::getBandwidthBurstRate()
{
  quint32 val = 0;
  with_torrc_value(SETTING_BANDWIDTH_BURST) {
    val = ret.at(0).trimmed().split(" ").at(0).toUInt();
  } else {
    val = defaultValue(SETTING_BANDWIDTH_BURST).toUInt();
  }
  return val;
}

/** Sets the maximum bandwidth burst rate (in KB/s) for this server. */
void
ServerSettings::setBandwidthBurstRate(quint32 rate)
{
  setVolatileValue(SETTING_BANDWIDTH_BURST, rate);
}

/** Sets whether the user's server descriptor will be published or not.
 * Currently this only affects publishing of bridge descriptors. If the
 * user is running a normal relay, its descriptor will always be
 * published regardless of this setting. */
void
ServerSettings::setPublishServerDescriptor(bool publish)
{
  setVolatileValue(SETTING_PUBLISH_SERVER_DESCRIPTOR, publish?"1":"0");
}

/** Returns true if the user's server descriptor will be published to the
 * appropriate authorities. */
bool
ServerSettings::publishServerDescriptor()
{
  with_torrc_value(SETTING_PUBLISH_SERVER_DESCRIPTOR) {
    return ret.at(0) != "0";
  } else {
    return defaultValue(SETTING_PUBLISH_SERVER_DESCRIPTOR).toBool();
  }
  return false;
}

/** Returns true if UPnP support is available and enabled. */
bool
ServerSettings::isUpnpEnabled()
{
#if defined(USE_MINIUPNPC)
  return localValue(SETTING_ENABLE_UPNP).toBool();
#else
  return false;
#endif
}

/** Sets whether Vidalia should try to configure port forwarding using UPnP.
 * If Vidalia was compiled without UPnP support, this method has no effect. */
void
ServerSettings::setUpnpEnabled(bool enabled)
{
#if defined(USE_MINIUPNPC)
  setValue(SETTING_ENABLE_UPNP, enabled);
#endif
}

bool
ServerSettings::isAccountingEnabled()
{
  with_torrc_value(SETTING_ACCOUNTING_MAX) {
    return true;
  }

  return false;
}

void
ServerSettings::disableAccounting()
{
  Vidalia::torrc()->clear(QStringList() << SETTING_ACCOUNTING_MAX);
}

void
ServerSettings::setAccountingMax(int amount, const QString &unit)
{
  setVolatileValue(SETTING_ACCOUNTING_MAX, QString("%1 %2").arg(amount).arg(unit));
}

void
ServerSettings::setAccountingStart(const QString &dwm,
                                   int day,
                                   const QString &time)
{
  if(dwm == "month" or dwm == "week")
    setVolatileValue(SETTING_ACCOUNTING_START,
                     QString("%1 %2 %3").arg(dwm)
                     .arg(day).arg(time));
  else
    setVolatileValue(SETTING_ACCOUNTING_START,
                     QString("%1 %2").arg(dwm).arg(time));
}

int
ServerSettings::accountingMaxAmount()
{
  with_torrc_value(SETTING_ACCOUNTING_MAX) {
    QStringList parts = ret.at(0).split(" ");
    if(parts.size() < 2)
      return 0;
    return parts.at(0).trimmed().toInt();
  }
  
  return 0;
}

const QString
ServerSettings::accountingMaxUnit()
{
  with_torrc_value(SETTING_ACCOUNTING_MAX) {
    QStringList parts = ret.at(0).split(" ");
    if(parts.size() < 2)
      return QString();
    return parts.at(1).trimmed();
  }
  
  return QString();
}

const QString
ServerSettings::accountingStartDwm()
{
  with_torrc_value(SETTING_ACCOUNTING_START) {
    QStringList parts = ret.at(0).split(" ");
    if(parts.size() < 2)
      return QString();
    return parts.at(0).trimmed();
  }
  
  return QString();
}

int
ServerSettings::accountingStartDay()
{
  with_torrc_value(SETTING_ACCOUNTING_START) {
    QStringList parts = ret.at(0).split(" ");
    if(parts.size() < 3)
      return 1;
    return parts.at(1).trimmed().toInt();
  }
  
  return 1;
}

const QString
ServerSettings::accountingStartTime()
{
  with_torrc_value(SETTING_ACCOUNTING_START) {
    QStringList parts = ret.at(0).split(" ");
    if(parts.size() < 2)
      return QString();
    if(parts.size() == 3)
      return parts.at(2).trimmed();
    return parts.at(1).trimmed();
  }
  
  return QString();
}
