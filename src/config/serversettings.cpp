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

/* Server-related torrc configuration parameters */
#define SERVER_NICKNAME         "Nickname"
#define SERVER_ORPORT           "ORPort"
#define SERVER_DIRPORT          "DirPort"
#define SERVER_CONTACTINFO      "ContactInfo"
#define SERVER_EXITPOLICY       "ExitPolicy"
#define SERVER_BANDWIDTH_RATE   "BandwidthRate"
#define SERVER_BANDWIDTH_BURST  "BandwidthBurst"
#define SERVER_RELAY_BANDWIDTH_RATE   "RelayBandwidthRate"
#define SERVER_RELAY_BANDWIDTH_BURST  "RelayBandwidthBurst"

/* Server configuration settings */
#define SETTING_SERVER_ENABLED    "Server/Enabled"
#define SETTING_SERVER_CHANGED    "Server/Changed"
#define SETTING_SERVER_DIRMIRROR  "Server/DirectoryMirror"
#define SETTING_SERVER_NICKNAME   "Server/"SERVER_NICKNAME
#define SETTING_SERVER_ORPORT     "Server/"SERVER_ORPORT
#define SETTING_SERVER_DIRPORT    "Server/"SERVER_DIRPORT
#define SETTING_SERVER_CONTACT    "Server/"SERVER_CONTACTINFO
#define SETTING_SERVER_EXITPOLICY "Server/"SERVER_EXITPOLICY
#define SETTING_SERVER_BWRATE     "Server/"SERVER_BANDWIDTH_RATE
#define SETTING_SERVER_BWBURST    "Server/"SERVER_BANDWIDTH_BURST


/** Constructor.
 * \param torControl a TorControl object used to read and apply the server
 * configuration settings.
 */
ServerSettings::ServerSettings(TorControl *torControl)
{
  _torControl = torControl;
  _backupSettings = allSettings();

  setDefault(SETTING_SERVER_ENABLED,    false);
  setDefault(SETTING_SERVER_CHANGED,    false);
  setDefault(SETTING_SERVER_DIRMIRROR,  true);
  setDefault(SETTING_SERVER_ORPORT,     9001);
  setDefault(SETTING_SERVER_DIRPORT,    9030);
  setDefault(SETTING_SERVER_CONTACT,    "<your@email.com>");
  setDefault(SETTING_SERVER_BWRATE,     3145728);
  setDefault(SETTING_SERVER_BWBURST,    6291456);
  setDefault(SETTING_SERVER_NICKNAME,   "Unnamed");
  setDefault(SETTING_SERVER_EXITPOLICY,
    ExitPolicy(ExitPolicy::Default).toString());
}

/** Stores a boolean value indicating if the server's configuration has
 * changed since it was last applied.
 * \param changed Boolean value indicating whether settings have been changed.
 */
void
ServerSettings::setChanged(bool changed)
{
  VidaliaSettings::setValue(SETTING_SERVER_CHANGED, changed);
}

/** Returns a boolean value indicating if the server's configuration has
 * changed since it was last applied.
 */
bool
ServerSettings::changedSinceLastApply()
{
  return VidaliaSettings::value(SETTING_SERVER_CHANGED).toBool();
}

/** Restores the server configuration back to its state after the last call to
 * apply().
 * \sa apply()
 */
void
ServerSettings::revert()
{
  beginGroup("Server");
  remove(""); /* Removes all settings in the Server group */
  foreach(QString key, _backupSettings.keys()) {
    setValue(key, _backupSettings.value(key));
  }
  endGroup();
}

/** Returns a map of all server configuration settings currently stored in
 * Vidalia's settings file. */
QMap<QString, QVariant>
ServerSettings::allSettings()
{
  QMap<QString, QVariant> settings;
  beginGroup("Server");
  foreach(QString key, allKeys()) {
    settings.insert(key, QSettings::value(key));
  }
  endGroup();
  return settings;
}

/** Returns true if the given QVariant contains an empty value, depending on
 * the data type. For example, 0 is considered an empty integer and "" is an
 * empty string. */
bool
ServerSettings::isEmptyValue(QVariant value)
{
  switch (value.type()) {
    case QVariant::String: 
      return (value.toString().isEmpty());
    case QVariant::UInt:
    case QVariant::Int:
      return (value.toUInt() == 0);
    case QVariant::Invalid:
      return true;
    default:  break;
  }
  return false;
}

/** Returns the stored value for the given key. If no stored value exists for
 * the given key, the specified default value is used. If Vidalia is currently
 * connected to Tor, we will ask Tor what its value is. Otherwise, we will
 * retrieve it from Vidalia's stored settings, allowing the configuration
 * information to be edited even if Tor isn't running.
 * \param key Configuration key
 * \param defaultValue Value to use if no configuration value exists for the
 * specified key.
 */
QVariant
ServerSettings::value(QString key)
{
  QVariant value;
  QString confKey, confValue;
  confKey = key.mid(key.indexOf("/")+1);
  if (_torControl->isConnected()) {
    quint32 torVersion = _torControl->getTorVersion();
    if (torVersion >= 0x020001) {
      if (confKey == SERVER_BANDWIDTH_RATE)
        confKey = SERVER_RELAY_BANDWIDTH_RATE;
      else if (confKey == SERVER_BANDWIDTH_BURST)
        confKey = SERVER_RELAY_BANDWIDTH_BURST;
    }
    if (_torControl->getConf(confKey, confValue)) {
      /* Get the value from Tor */
      value.setValue(confValue);
      value.convert(defaultValue(key).type());
    }
  } else {
    /* Read our saved value from vidalia.conf */
    value = VidaliaSettings::value(key);
  }
  return (isEmptyValue(value) ? defaultValue(key) : value);
}

/** Saves the given configuration key-value to the application settings file.
 * \param key Configuration key
 * \param value Value to assign to <b>key</b>
 */
void
ServerSettings::setValue(QString key, QVariant value)
{
  if (value != VidaliaSettings::value(key)) {
    setChanged(true);
    VidaliaSettings::setValue(key, value);
  }
}

/** Returns a QHash of Tor-recognizable configuratin keys to their current
 * values. */
QHash<QString, QString>
ServerSettings::confValues()
{
  QHash<QString, QString> conf;
  quint32 torVersion = _torControl->getTorVersion();

  /* Server Nickname */
  conf.insert(SERVER_NICKNAME,
    (isServerEnabled() ? VidaliaSettings::value(SETTING_SERVER_NICKNAME).toString()
                       : ""));
  /* Server ORPort */
  conf.insert(SERVER_ORPORT,
    (isServerEnabled() ? VidaliaSettings::value(SETTING_SERVER_ORPORT).toString()
                       : "0"));
  /* Server DirPort */
  conf.insert(SERVER_DIRPORT, 
    (isDirectoryMirror() ? VidaliaSettings::value(SETTING_SERVER_DIRPORT).toString() 
                         : "0"));
  /* Server Exit Policy */
  conf.insert(SERVER_EXITPOLICY, 
    VidaliaSettings::value(SETTING_SERVER_EXITPOLICY).toString());
  
  /* Server bandwidth settings */
  conf.insert((torVersion >= 0x020001 ? SERVER_RELAY_BANDWIDTH_RATE 
                                      : SERVER_BANDWIDTH_RATE),
    QString::number(VidaliaSettings::value(SETTING_SERVER_BWRATE).toUInt()) + " bytes");
  conf.insert((torVersion >= 0x020001 ? SERVER_RELAY_BANDWIDTH_BURST
                                      : SERVER_BANDWIDTH_BURST),
    QString::number(VidaliaSettings::value(SETTING_SERVER_BWBURST).toUInt()) + " bytes");
    
  /* Server Contact Information */
  QString contact = 
    VidaliaSettings::value(SETTING_SERVER_CONTACT).toString().trimmed();
  QString defaultContact =
    VidaliaSettings::defaultValue(SETTING_SERVER_CONTACT).toString();
  if ((contact == defaultContact) ||
      (contact == scrub_email_addr(defaultContact))) {
    /* Only set the contact info if they put something non-default there */
    contact = "";
  }
  conf.insert(SERVER_CONTACTINFO, scrub_email_addr(contact));
  return conf;
}

/** Applies the current server configuration settings to Tor and tells Tor to
 * write the new configuration to disk. 
 * \param errmsg If specified, will store an error message should something go
 * wrong. 
 */
bool
ServerSettings::apply(QString *errmsg)
{
  bool rc;

  if (isServerEnabled()) {
    rc = _torControl->setConf(confValues(), errmsg);
  } else { 
    QStringList resetKeys;
    quint32 torVersion = _torControl->getTorVersion();
    resetKeys << SERVER_ORPORT 
              << SERVER_NICKNAME 
              << SERVER_DIRPORT
              << SERVER_CONTACTINFO
              << SERVER_EXITPOLICY;
    if (torVersion >= 0x020001) {
      resetKeys << SERVER_RELAY_BANDWIDTH_RATE
                << SERVER_RELAY_BANDWIDTH_BURST;
    } else {
      resetKeys << SERVER_BANDWIDTH_RATE
                << SERVER_BANDWIDTH_BURST;
    }
    rc = _torControl->resetConf(resetKeys, errmsg);
  }
  if (rc) {
    if (_torControl->saveConf(errmsg))
      setChanged(false);
    _backupSettings = allSettings();
  }
  return rc;
}

/** Enables or disables running Tor as a server. 
 * \param enable Whether to enable or disable the Tor server. 
 */
void
ServerSettings::setServerEnabled(bool enable)
{
  setValue(SETTING_SERVER_ENABLED, enable);
}

/** Returns true if Tor is currently configured to run as a Tor server. If Tor
 * was started with a specific torrc, we will ask Tor whether that torrc told
 * it to be a server. Otherwise, we use our local settings. */
bool
ServerSettings::isServerEnabled()
{
  TorSettings settings;
  QHash<QString,QString> confValues;
  if (_torControl->isConnected() && !changedSinceLastApply()) {
    confValues.insert(SERVER_ORPORT, "");
    confValues.insert(SERVER_NICKNAME, "");
    if (_torControl->getConf(confValues)) {
      return (confValues.value(SERVER_ORPORT).toUInt() != 0 &&
              !confValues.value(SERVER_NICKNAME).isEmpty());
    }
  }
  return VidaliaSettings::value(SETTING_SERVER_ENABLED).toBool();
}

/** Sets the server's ORPort. */
void
ServerSettings::setORPort(quint16 orPort)
{
  setValue(SETTING_SERVER_ORPORT, orPort);
}

/** Gets the server's current ORPort setting. */
quint16
ServerSettings::getORPort()
{
  return (quint16)value(SETTING_SERVER_ORPORT).toUInt();
}

/** Sets the server's current DirPort. */
void
ServerSettings::setDirPort(quint16 dirPort)
{
  setValue(SETTING_SERVER_DIRPORT, dirPort);
}

/** Gets the server's current DirPort. */
quint16
ServerSettings::getDirPort()
{
  return (quint16)value(SETTING_SERVER_DIRPORT).toUInt();
}

/** Sets the server's nickname. */
void
ServerSettings::setNickname(QString nickname)
{
  setValue(SETTING_SERVER_NICKNAME, nickname);
}

/** Gets the server's nickname. */
QString
ServerSettings::getNickname()
{
  QString nickname = value(SETTING_SERVER_NICKNAME).toString();
  /* Ensure the nickname contains only valid characters and is not too long. */
  return ensure_valid_chars(nickname, 
                            VALID_NICKNAME_CHARS).left(MAX_NICKNAME_LEN);
}

/** Sets the server's contact information. */
void
ServerSettings::setContactInfo(QString contact)
{
  setValue(SETTING_SERVER_CONTACT, contact);
}

/** Gets the server's contact information. */
QString
ServerSettings::getContactInfo()
{
  return value(SETTING_SERVER_CONTACT).toString();
}

/** Returns whether this server will act as a directory mirror or not. */
bool
ServerSettings::isDirectoryMirror()
{
  return VidaliaSettings::value(SETTING_SERVER_DIRMIRROR).toBool();
}

/** Sets whether this server will act as a directory mirror. */
void
ServerSettings::setDirectoryMirror(bool mirror)
{
  setValue(SETTING_SERVER_DIRMIRROR, mirror);
}

/** Returns the exit policy for this server. */
ExitPolicy
ServerSettings::getExitPolicy()
{
  return ExitPolicy(value(SETTING_SERVER_EXITPOLICY).toString());
}

/** Sets the exit policy for this server. */
void
ServerSettings::setExitPolicy(ExitPolicy &exitPolicy)
{
  setValue(SETTING_SERVER_EXITPOLICY, exitPolicy.toString());
}

/** Returns the long-term average bandwidth rate (in KB/s) for this server. */
quint32
ServerSettings::getBandwidthAvgRate()
{
  return value(SETTING_SERVER_BWRATE).toUInt();
}

/** Sets the long-term average bandwidth rate (in KB/s) for this server. */
void
ServerSettings::setBandwidthAvgRate(quint32 rate)
{
  setValue(SETTING_SERVER_BWRATE, rate);
}

/** Returns the maximum bandwidth burst rate (in KB/s) for this server. */
quint32
ServerSettings::getBandwidthBurstRate()
{
  return value(SETTING_SERVER_BWBURST).toUInt();
}

/** Sets the maximum bandwidth burst rate (in KB/s) for this server. */
void
ServerSettings::setBandwidthBurstRate(quint32 rate)
{
  setValue(SETTING_SERVER_BWBURST, rate);
}

