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
 * \file serversettings.cpp
 * \version $Id$
 * \brief Settings for running a Tor server
 */

#include <QHostInfo>
#include <util/net.h>
#include <util/string.h>

#include "serversettings.h"
#include "torsettings.h"

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
#define SETTING_RELAY_BANDWIDTH_RATE   "RelayBandwidthRate"
#define SETTING_RELAY_BANDWIDTH_BURST  "RelayBandwidthBurst"
#define SETTING_PUBLISH_DESCRIPTOR     "PublishServerDescriptor"


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
  setDefault(SETTING_CONTACT,       "<your@email.com>");
  setDefault(SETTING_BANDWIDTH_RATE,        5242880);
  setDefault(SETTING_RELAY_BANDWIDTH_RATE,  5242880);
  setDefault(SETTING_BANDWIDTH_BURST,       10485760);
  setDefault(SETTING_RELAY_BANDWIDTH_BURST, 10485760);
  setDefault(SETTING_PUBLISH_DESCRIPTOR,    "1");
  setDefault(SETTING_EXITPOLICY,
    ExitPolicy(ExitPolicy::Default).toString());
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
  
  /* If we're a bridge, don't publish our server descriptor */
  conf.insert(SETTING_PUBLISH_DESCRIPTOR,
              (torVersion >= 0x020008 && isBridgeEnabled() ? "bridge" : "1"));

  return conf;
}

/** Applies the current server configuration settings to Tor. If <b>errmsg</b>
 * is specified and an error occurs while applying the settings, it will be 
 * set to a string describing the error. */
bool
ServerSettings::apply(QString *errmsg)
{
  bool rc;

  if (isServerEnabled()) {
    rc = torControl()->setConf(confValues(), errmsg);
  } else { 
    QStringList resetKeys;
    quint32 torVersion = torControl()->getTorVersion();
    resetKeys << SETTING_ORPORT 
              << SETTING_NICKNAME 
              << SETTING_DIRPORT
              << SETTING_CONTACT
              << SETTING_EXITPOLICY;
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

/** Virtual method called when we retrieve a server-related setting from Tor.
 * Currently this just translates BandwidthFoo to RelayBandwidthFoo when
 * appropriate. */
QVariant
ServerSettings::torValue(const QString &key)
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
  setValue(SETTING_PUBLISH_DESCRIPTOR, enabled ? "bridge" : "1");
}

/** Returns true if Tor is configured to act as a bridge node. */
bool
ServerSettings::isBridgeEnabled()
{
  return (isServerEnabled() && 
          value(SETTING_PUBLISH_DESCRIPTOR).toString().toLower() == "bridge");
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

