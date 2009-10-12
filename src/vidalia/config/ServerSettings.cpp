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
** \file ServerSettings.cpp
** \version $Id$
** \brief Settings for running a Tor server
*/

#include "config.h"
#include "ServerSettings.h"
#include "TorSettings.h"
#ifdef USE_MINIUPNPC
#include "UPNPControl.h"
#endif

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
#define SETTING_ENABLE_UPNP     "EnableUPnP"
#define SETTING_RELAY_BANDWIDTH_RATE   "RelayBandwidthRate"
#define SETTING_RELAY_BANDWIDTH_BURST  "RelayBandwidthBurst"
#define SETTING_PUBLISH_SERVER_DESCRIPTOR "PublishServerDescriptor"


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
    ExitPolicy(ExitPolicy::Default).toString());
  setDefault(SETTING_ENABLE_UPNP, false); 
  setDefault(SETTING_BRIDGE_RELAY, false);
  setDefault(SETTING_PUBLISH_SERVER_DESCRIPTOR, "1");
}

/** Returns a QHash of Tor-recognizable configuratin keys to their current
 * values. */
QHash<QString, QString>
ServerSettings::confValues()
{
  QHash<QString, QString> conf;
  quint32 torVersion = torControl()->getTorVersion();

  /* Server Nickname */
  conf.insert(SETTING_NICKNAME,
    (isServerEnabled() ? localValue(SETTING_NICKNAME).toString()
                       : ""));
  /* Server ORPort */
  conf.insert(SETTING_ORPORT,
    (isServerEnabled() ? localValue(SETTING_ORPORT).toString()
                       : "0"));
  /* Server DirPort */
  conf.insert(SETTING_DIRPORT, 
    (isDirectoryMirror() ? localValue(SETTING_DIRPORT).toString() 
                         : "0"));
  /* Server Exit Policy */
  conf.insert(SETTING_EXITPOLICY, 
    (isBridgeEnabled() ? "reject *:*"
                       : localValue(SETTING_EXITPOLICY).toString()));
  
  /* Server bandwidth settings */
  conf.insert((torVersion >= 0x020001 ? SETTING_RELAY_BANDWIDTH_RATE 
                                      : SETTING_BANDWIDTH_RATE),
    QString::number(localValue(SETTING_BANDWIDTH_RATE).toUInt()) + " bytes");
  conf.insert((torVersion >= 0x020001 ? SETTING_RELAY_BANDWIDTH_BURST
                                      : SETTING_BANDWIDTH_BURST),
    QString::number(localValue(SETTING_BANDWIDTH_BURST).toUInt()) + " bytes");
    
  /* Server Contact Information */
  QString contact = 
    localValue(SETTING_CONTACT).toString().trimmed();
  QString defaultContact = defaultValue(SETTING_CONTACT).toString();
  if ((contact == defaultContact) ||
      (contact == scrub_email_addr(defaultContact))) {
    /* Only set the contact info if they put something non-default there */
    contact = "";
  }
  conf.insert(SETTING_CONTACT, scrub_email_addr(contact));
  
  /* Set if we're a bridge relay */
  if (isBridgeEnabled()) {
    conf.insert(SETTING_BRIDGE_RELAY, "1");
    conf.insert(SETTING_PUBLISH_SERVER_DESCRIPTOR,
                publishServerDescriptor() ? "1" : "0");
  } else {
    conf.insert(SETTING_BRIDGE_RELAY, "0");
    conf.insert(SETTING_PUBLISH_SERVER_DESCRIPTOR, "1");
  }
  return conf;
}

/** Applies the current server configuration settings to Tor. If <b>errmsg</b>
 * is specified and an error occurs while applying the settings, it will be 
 * set to a string describing the error. */
bool
ServerSettings::apply(QString *errmsg)
{
  bool rc;

  configurePortForwarding();

  if (isServerEnabled()) {
    rc = torControl()->setConf(confValues(), errmsg);
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
    rc = torControl()->resetConf(resetKeys, errmsg);
  }
  return rc;
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
  setValue(SETTING_ENABLED, enable);
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
  return localValue(SETTING_ENABLED).toBool();
}

/** Sets to <b>enabled</b> whether Tor should be a bridge node when acting as
 * a server. */
void
ServerSettings::setBridgeEnabled(bool enabled)
{
  setValue(SETTING_BRIDGE_RELAY, enabled);
}

/** Returns true if Tor is configured to act as a bridge node. */
bool
ServerSettings::isBridgeEnabled()
{
  return value(SETTING_BRIDGE_RELAY).toBool() && isServerEnabled();
}

/** Sets the server's ORPort. */
void
ServerSettings::setORPort(quint16 orPort)
{
  setValue(SETTING_ORPORT, orPort);
}

/** Gets the server's current ORPort setting. */
quint16
ServerSettings::getORPort()
{
  return (quint16)value(SETTING_ORPORT).toUInt();
}

/** Sets the server's current DirPort. */
void
ServerSettings::setDirPort(quint16 dirPort)
{
  setValue(SETTING_DIRPORT, dirPort);
}

/** Gets the server's current DirPort. */
quint16
ServerSettings::getDirPort()
{
  return (quint16)value(SETTING_DIRPORT).toUInt();
}

/** Sets the server's nickname. */
void
ServerSettings::setNickname(QString nickname)
{
  setValue(SETTING_NICKNAME, nickname);
}

/** Gets the server's nickname. */
QString
ServerSettings::getNickname()
{
  QString nickname = value(SETTING_NICKNAME).toString();
  /* Ensure the nickname contains only valid characters and is not too long. */
  return ensure_valid_chars(nickname, 
                            VALID_NICKNAME_CHARS).left(MAX_NICKNAME_LEN);
}

/** Sets the server's contact information. */
void
ServerSettings::setContactInfo(QString contact)
{
  setValue(SETTING_CONTACT, contact);
}

/** Gets the server's contact information. */
QString
ServerSettings::getContactInfo()
{
  return value(SETTING_CONTACT).toString();
}

/** Returns whether this server will act as a directory mirror or not. */
bool
ServerSettings::isDirectoryMirror()
{
  return localValue(SETTING_DIRMIRROR).toBool();
}

/** Sets whether this server will act as a directory mirror. */
void
ServerSettings::setDirectoryMirror(bool mirror)
{
  setValue(SETTING_DIRMIRROR, mirror);
}

/** Returns the exit policy for this server. */
ExitPolicy
ServerSettings::getExitPolicy()
{
  return ExitPolicy(value(SETTING_EXITPOLICY).toString());
}

/** Sets the exit policy for this server. */
void
ServerSettings::setExitPolicy(ExitPolicy &exitPolicy)
{
  setValue(SETTING_EXITPOLICY, exitPolicy.toString());
}

/** Returns the long-term average bandwidth rate (in KB/s) for this server. */
quint32
ServerSettings::getBandwidthAvgRate()
{
  return value(SETTING_BANDWIDTH_RATE).toUInt();
}

/** Sets the long-term average bandwidth rate (in KB/s) for this server. */
void
ServerSettings::setBandwidthAvgRate(quint32 rate)
{
  setValue(SETTING_BANDWIDTH_RATE, rate);
}

/** Returns the maximum bandwidth burst rate (in KB/s) for this server. */
quint32
ServerSettings::getBandwidthBurstRate()
{
  return value(SETTING_BANDWIDTH_BURST).toUInt();
}

/** Sets the maximum bandwidth burst rate (in KB/s) for this server. */
void
ServerSettings::setBandwidthBurstRate(quint32 rate)
{
  setValue(SETTING_BANDWIDTH_BURST, rate);
}

/** Sets whether the user's server descriptor will be published or not.
 * Currently this only affects publishing of bridge descriptors. If the
 * user is running a normal relay, its descriptor will always be
 * published regardless of this setting. */
void
ServerSettings::setPublishServerDescriptor(bool publish)
{
  if (publish)
    setValue(SETTING_PUBLISH_SERVER_DESCRIPTOR, "1");
  else
    setValue(SETTING_PUBLISH_SERVER_DESCRIPTOR, "0");
}

/** Returns true if the user's server descriptor will be published to the
 * appropriate authorities. */
bool
ServerSettings::publishServerDescriptor() const
{
  return (value(SETTING_PUBLISH_SERVER_DESCRIPTOR).toString() != "0"); 
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

