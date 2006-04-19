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
 */

#include <QHostInfo>
#include <util/net.h>

#include "serversettings.h"
#include "torsettings.h"


/* Server-related torrc configuration parameters */
#define SERVER_NICKNAME       "Nickname"
#define SERVER_ADDRESS        "Address"
#define SERVER_ORPORT         "ORPort"
#define SERVER_DIRPORT        "DirPort"
#define SERVER_CONTACTINFO    "ContactInfo"
#define SERVER_EXITPOLICY     "ExitPolicy"

/* Server configuration settings */
#define SETTING_SERVER_ENABLED    "Server/Enabled"
#define SETTING_SERVER_CHANGED    "Server/Changed"
#define SETTING_SERVER_DIRMIRROR  "Server/DirectoryMirror"
#define SETTING_SERVER_AUTOUPDATE_ADDRESS "Server/AutoUpdateAddress"
#define SETTING_SERVER_MIDDLEMAN  "Server/Middleman"
#define SETTING_SERVER_NICKNAME   "Server/"SERVER_NICKNAME
#define SETTING_SERVER_ORPORT     "Server/"SERVER_ORPORT
#define SETTING_SERVER_DIRPORT    "Server/"SERVER_DIRPORT
#define SETTING_SERVER_ADDRESS    "Server/"SERVER_ADDRESS
#define SETTING_SERVER_CONTACT    "Server/"SERVER_CONTACTINFO
#define SETTING_SERVER_EXITPOLICY "Server/"SERVER_EXITPOLICY

/* Default server configuration */
#define DEFAULT_SERVER_ENABLED    false
#define DEFAULT_SERVER_CHANGED    false
#define DEFAULT_SERVER_DIRMIRROR  false
#define DEFAULT_SERVER_MIDDLEMAN  false
#define DEFAULT_SERVER_NICKNAME   QHostInfo::localHostName() 
#define DEFAULT_SERVER_ORPORT     9001
#define DEFAULT_SERVER_DIRPORT    9030
#define DEFAULT_SERVER_CONTACT    "<your@email.com>"
#define DEFAULT_SERVER_ADDRESS    net_local_address().toString() 
#define DEFAULT_SERVER_EXITPOLICY ""
#define DEFAULT_SERVER_AUTOUPDATE_ADDRESS false


/** Constructor.
 * \param torControl a TorControl object used to read and apply the server
 * configuration settings.
 */
ServerSettings::ServerSettings(TorControl *torControl)
{
  _torControl = torControl;
  _backupSettings = allSettings();
}

/** Stores a boolean value indicating if the server's configuration has
 * changed since it was last applied.
 * \param changed Boolean value indicating whether settings have been changed.
 */
void
ServerSettings::setChanged(bool changed)
{
  QSettings::setValue(SETTING_SERVER_CHANGED, changed);
}

/** Returns a boolean value indicating if the server's configuration has
 * changed since it was last applied.
 */
bool
ServerSettings::changedSinceLastApply()
{
  return QSettings::value(SETTING_SERVER_CHANGED,
                          DEFAULT_SERVER_CHANGED).toBool();
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
    case QVariant::String: return (value.toString().isEmpty());
    case QVariant::UInt:  
    case QVariant::Int:   return (value.toUInt() == 0);
    default:  break;
  }
  return false;
}

/** Returns the stored value for the given key. If no stored value exists for
 * the given key, the specified default value is used. If Vidalia is currently
 * connected to Tor, we will ask Tor what it's value is. Otherwise, we will
 * retrieve it from Vidalia's stored settings, allowing the configuration
 * information to be edited even if Tor isn't running.
 * \param key Configuration key
 * \param defaultValue Value to use if no configuration value exists for the
 * specified key.
 */
QVariant
ServerSettings::value(QString key, QVariant defaultValue)
{
  QVariant value;
  QString confKey, confValue;
  confKey = key.mid(key.indexOf("/")+1);
  if (_torControl->isConnected()) {
    if (_torControl->getConf(confKey, confValue)) {
      value.setValue(confValue);
      value.convert(defaultValue.type());
    }
  } else {
    value = QSettings::value(key, defaultValue);
  }
  return (isEmptyValue(value) ? defaultValue : value);
}

/** Saves the given configuration key-value to the application settings file.
 * \param key Configuration key
 * \param value Value to assign to <b>key</b>
 */
void
ServerSettings::setValue(QString key, QVariant value)
{
  setChanged(true);
  QSettings::setValue(key, value);
}

/** Returns a QHash of Tor-recognizable configuratin keys to their current
 * values. */
QHash<QString, QString>
ServerSettings::confValues()
{
  QHash<QString, QString> conf;
  /* Server Nickname */
  conf.insert(SERVER_NICKNAME,
    (isServerEnabled() ? QSettings::value(SETTING_SERVER_NICKNAME, 
                                          DEFAULT_SERVER_NICKNAME).toString()
                       : ""));
  /* Server ORPort */
  conf.insert(SERVER_ORPORT,
    (isServerEnabled() ? QSettings::value(SETTING_SERVER_ORPORT, 
                                          DEFAULT_SERVER_ORPORT).toString()
                       : "0"));
  /* Server DirPort */
  conf.insert(SERVER_DIRPORT, 
    (isDirectoryMirror() ? QSettings::value(SETTING_SERVER_DIRPORT, 
                                            DEFAULT_SERVER_DIRPORT).toString() 
                         : "0"));
  /* Server Exit Policy */
  conf.insert(SERVER_EXITPOLICY, 
    (isMiddleman() ? ExitPolicy(ExitPolicy::Middleman).toString()
                   : QSettings::value(SETTING_SERVER_EXITPOLICY,
                                      DEFAULT_SERVER_EXITPOLICY).toString()));
  /* Server Address */
  conf.insert(SERVER_ADDRESS,      
    QSettings::value(SETTING_SERVER_ADDRESS, DEFAULT_SERVER_ADDRESS).toString());
  
  /* Server Contact Information */
  conf.insert(SERVER_CONTACTINFO, 
    QSettings::value(SETTING_SERVER_CONTACT, DEFAULT_SERVER_CONTACT).toString());
  
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
    resetKeys << SERVER_ORPORT << SERVER_NICKNAME;
    rc = _torControl->resetConf(resetKeys, errmsg);
  }
  if (rc) {
    if (!_torControl->saveConf(errmsg)) {
      return false;
    }
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
  if (_torControl->isConnected() && settings.usingAlternateTorrc()) {
    confValues.insert(SERVER_ORPORT, "");
    confValues.insert(SERVER_NICKNAME, "");
    if (_torControl->getConf(confValues)) {
      return (confValues.value(SERVER_ORPORT).toUInt() != 0 &&
              !confValues.value(SERVER_NICKNAME).isEmpty());
    }
  }
  return QSettings::value(SETTING_SERVER_ENABLED,
                          DEFAULT_SERVER_ENABLED).toBool();
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
  return (quint16)value(SETTING_SERVER_ORPORT, 
                        DEFAULT_SERVER_ORPORT).toUInt();
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
  return (quint16)value(SETTING_SERVER_DIRPORT, 
                        DEFAULT_SERVER_DIRPORT).toUInt();
}

/** Sets the server's externally-reachable address. */
void
ServerSettings::setAddress(QString address)
{
  setValue(SETTING_SERVER_ADDRESS, address);
}

/** Gets the server's externally-reachable IP address or hostname. */
QString
ServerSettings::getAddress()
{
  return value(SETTING_SERVER_ADDRESS, DEFAULT_SERVER_ADDRESS).toString();
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
  return value(SETTING_SERVER_NICKNAME, DEFAULT_SERVER_NICKNAME).toString();
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
  return value(SETTING_SERVER_CONTACT, DEFAULT_SERVER_CONTACT).toString();
}

/** Returns whether this server will act as a directory mirror or not. */
bool
ServerSettings::isDirectoryMirror()
{
  return QSettings::value(SETTING_SERVER_DIRMIRROR, 
                          DEFAULT_SERVER_DIRMIRROR).toBool();
}

/** Sets whether this server will act as a directory mirror. */
void
ServerSettings::setDirectoryMirror(bool mirror)
{
  setValue(SETTING_SERVER_DIRMIRROR, mirror);
}

/** Returns whether this server is a middle-man server. A middle-man server
 * simply relays traffic between other Tor servers. A middle-man server can
 * also act as an entry node for Tor clients. */
bool
ServerSettings::isMiddleman()
{
  return QSettings::value(SETTING_SERVER_MIDDLEMAN,
                          DEFAULT_SERVER_MIDDLEMAN).toBool();
}

/** Sets whether this server will act as a middle-man server. */
void
ServerSettings::setMiddleman(bool middleman)
{
  setValue(SETTING_SERVER_MIDDLEMAN, middleman);
}

/** Returns the exit policy for this server. */
ExitPolicy
ServerSettings::getExitPolicy()
{
  return ExitPolicy(value(SETTING_SERVER_EXITPOLICY, 
                          DEFAULT_SERVER_EXITPOLICY).toString());
}

/** Sets the exit policy for this server. */
void
ServerSettings::setExitPolicy(ExitPolicy &exitPolicy)
{
  setValue(SETTING_SERVER_EXITPOLICY, exitPolicy.toString());
}

/** Returns whether we should update the server's IP address automatically. */
bool
ServerSettings::getAutoUpdateAddress()
{
  return QSettings::value(SETTING_SERVER_AUTOUPDATE_ADDRESS,
                          DEFAULT_SERVER_AUTOUPDATE_ADDRESS).toBool();
}

/** Sets whether we should update the server's IP address automatically. */
void
ServerSettings::setAutoUpdateAddress(bool enabled)
{
  setValue(SETTING_SERVER_AUTOUPDATE_ADDRESS, enabled);
}

