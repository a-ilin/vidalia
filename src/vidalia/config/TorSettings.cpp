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
** \file TorSettings.cpp
** \brief Settings used for starting and running Tor
*/

#include "TorSettings.h"
#include "Vidalia.h"
#include "crypto.h"
#include "file.h"
#include "stringutil.h"
#if defined(Q_OS_WIN32)
#include "win32.h"
#include <QFileInfo>
#endif

#include <QDir>
#include <QProcess>

/* Tor Settings */
#define SETTING_TOR_EXECUTABLE      "TorExecutable"
#define SETTING_TORRC               "Torrc"
#define SETTING_DEFAULTS_TORRC      "DefaultsTorrc"
#define SETTING_CONTROL_ADDR        "ControlListenAddress"
#define SETTING_CONTROL_PORT        "ControlPort"
#define SETTING_SOCKS_PORT          "SocksPort"
#define SETTING_SOCKET_PATH         "ControlSocket"
#define SETTING_CONTROL_METHOD      "ControlMethod"
#define SETTING_AUTH_TOKEN          "AuthToken"
#define SETTING_TOR_USER            "User"
#define SETTING_TOR_GROUP           "Group"
#define SETTING_DATA_DIRECTORY      "DataDirectory"
#define SETTING_AUTH_METHOD         "AuthenticationMethod"
#define SETTING_CONTROL_PASSWORD    "ControlPassword"
#define SETTING_USE_RANDOM_PASSWORD "UseRandomPassword"
#define SETTING_WARN_PLAINTEXT_PORTS    "WarnPlaintextPorts"
#define SETTING_REJECT_PLAINTEXT_PORTS  "RejectPlaintextPorts"
#define SETTING_BOOTSTRAP            "Bootstrap"
#define SETTING_BOOTSTRAP_FROM       "BootstrapFrom"
#define SETTING_AUTOCONTROL          "AutoControl"

/** Default to using hashed password authentication */
#define DEFAULT_AUTH_METHOD     PasswordAuth
/** Default control method */
#define DEFAULT_CONTROL_METHOD  "ControlPort"
/** Default socket path */
#define DEFAULT_SOCKET_PATH  ""

/* Arguments we can pass to Tor on the command-line */
#define TOR_ARG_CONTROL_PORT    "ControlPort"
#define TOR_ARG_TORRC           "-f"
#define TOR_ARG_DATA_DIRECTORY  "DataDirectory"
#define TOR_ARG_HASHED_PASSWORD "HashedControlPassword"
#define TOR_ARG_COOKIE_AUTH     "CookieAuthentication"
#define TOR_ARG_SOCKSPORT       "SocksPort"
#define TOR_ARG_CONTROLFILE     "ControlPortWriteToFile"

/** Generate random control passwords of 16 characters */
#define PASSWORD_LEN    16


/** Default constructor */
TorSettings::TorSettings(TorControl *torControl)
: AbstractTorSettings("Tor", torControl)
{
#if defined(Q_OS_WIN32)
  QString programFiles = win32_program_files_folder();
  if (QFileInfo(programFiles + "\\Vidalia Bundle\\Tor\\tor.exe").exists())
    setDefault(SETTING_TOR_EXECUTABLE,
               programFiles + "\\Vidalia Bundle\\Tor\\tor.exe");
  else
    setDefault(SETTING_TOR_EXECUTABLE, programFiles + "\\Tor\\tor.exe");
#else
  setDefault(SETTING_TOR_EXECUTABLE, "tor");
#endif

  setDefault(SETTING_TORRC,         Vidalia::dataDirectory() + "/torrc");
  setDefault(SETTING_DEFAULTS_TORRC,Vidalia::dataDirectory() + "/defaults_torrc");
  setDefault(SETTING_CONTROL_ADDR,  "127.0.0.1");
  setDefault(SETTING_CONTROL_PORT,  9051);
  setDefault(SETTING_SOCKS_PORT,  9050);
  setDefault(SETTING_AUTH_METHOD,   toString(DEFAULT_AUTH_METHOD));
  setDefault(SETTING_CONTROL_METHOD, DEFAULT_CONTROL_METHOD);
  setDefault(SETTING_SOCKET_PATH, DEFAULT_SOCKET_PATH);
  setDefault(SETTING_DATA_DIRECTORY, "");
  setDefault(SETTING_CONTROL_PASSWORD, "");
  setDefault(SETTING_USE_RANDOM_PASSWORD, true);
  setDefault(SETTING_BOOTSTRAP, false);
  setDefault(SETTING_BOOTSTRAP_FROM, "");
  setDefault(SETTING_AUTOCONTROL, false);
}

/** Applies any changes to Tor's control port or authentication settings. */
bool
TorSettings::apply(QString *errmsg)
{
  Torrc *torrc = Vidalia::torrc();

  torrc->clear(QStringList()
               << TOR_ARG_SOCKSPORT
               << SETTING_CONTROL_PORT);

  torrc->setValue(SETTING_DATA_DIRECTORY, volatileValue(SETTING_DATA_DIRECTORY).toString());

  if(volatileValue(SETTING_AUTOCONTROL).toBool()) {
    torrc->setValue(SETTING_CONTROL_PORT, "auto");
    torrc->setValue(SETTING_SOCKS_PORT, "auto");
    torrc->setValue(TOR_ARG_CONTROLFILE, QString("%1/port.conf").arg(getDataDirectory()));
  } else {
    QString socks = volatileValue(SETTING_SOCKS_PORT).toString();
    QString control = volatileValue(SETTING_CONTROL_PORT).toString();
    {
      with_torrc_value(SETTING_SOCKS_PORT)
        socks = ret.at(0);
    }
    {
      with_torrc_value(SETTING_CONTROL_PORT)
        control = ret.at(0);
    }
    torrc->setValue(SETTING_SOCKS_PORT, socks);
    torrc->setValue(SETTING_CONTROL_PORT, control);
  }

  QString hashedPassword;
  AuthenticationMethod authMethod =
    toAuthenticationMethod(localValue(SETTING_AUTH_METHOD).toString());
  switch (authMethod) {
    case CookieAuth:
      torrc->setValue(TOR_ARG_COOKIE_AUTH,    "1");
      torrc->setValue(TOR_ARG_HASHED_PASSWORD, "");
      break;
    case PasswordAuth:
      hashedPassword = useRandomPassword()
                          ? hashPassword(randomPassword())
                          : hashPassword(getControlPassword());
      if (hashedPassword.isEmpty()) {
        if (errmsg)
          *errmsg =  tr("Failed to hash the control password.");
        return false;
      }
      torrc->setValue(TOR_ARG_COOKIE_AUTH,    "0");
      if(not useRandomPassword())
        torrc->setValue(TOR_ARG_HASHED_PASSWORD, hashedPassword);
      break;
    default:
      torrc->setValue(TOR_ARG_COOKIE_AUTH,    "0");
      torrc->setValue(TOR_ARG_HASHED_PASSWORD, "");
  }

  return torrc->apply(Vidalia::torControl(), errmsg);
}

/** Gets the location of Tor's data directory. */
QString
TorSettings::getDataDirectory() const
{
  with_torrc_value(SETTING_DATA_DIRECTORY) {
    return ret.at(0);
  }
  return QDir::convertSeparators(volatileValue(SETTING_DATA_DIRECTORY).toString());
}

/** Sets the location to use as Tor's data directory. */
void
TorSettings::setDataDirectory(const QString &dataDirectory)
{
  setVolatileValue(SETTING_DATA_DIRECTORY, dataDirectory);
}

/** Returns a fully-qualified path to Tor's executable, including the
 * executable name. */
QString
TorSettings::getExecutable() const
{
  QString tor = localValue(SETTING_TOR_EXECUTABLE).toString();
  if (tor.isEmpty()) /* Don't let the Tor executable name be empty */
    tor = defaultValue(SETTING_TOR_EXECUTABLE).toString();
  return QDir::convertSeparators(tor);
}

/** Sets the location and name of Tor's executable to the given string. */
void
TorSettings::setExecutable(const QString &torExecutable)
{
  setValue(SETTING_TOR_EXECUTABLE, torExecutable);
}

/** Returns the torrc that will be used when starting Tor. */
QString
TorSettings::getTorrc() const
{
  QString torrc;
  TorControl *tc = torControl();
  if (tc && tc->isConnected() && tc->getInfo("config-file", torrc))
    return QDir::convertSeparators(torrc);

  torrc = localValue(SETTING_TORRC).toString();
  if(QDir(QFileInfo(torrc).filePath()).isRelative())
    torrc = QCoreApplication::applicationDirPath() + "/" + torrc;
  return QDir::convertSeparators(torrc);
}

/** Sets the torrc that will be used when starting Tor.
 * \param torrc The torrc to use.
 */
void
TorSettings::setTorrc(const QString &torrc)
{
  setValue(SETTING_TORRC, torrc);
}

/** Returns the defaults torrc that will be used when starting Tor. */
QString
TorSettings::getDefaultsTorrc() const
{
  QString torrc;
  // TorControl *tc = torControl();
  // if (tc && tc->isConnected() && tc->getInfo("config-file", torrc))
  //   return QDir::convertSeparators(torrc);

  torrc = localValue(SETTING_DEFAULTS_TORRC).toString();
  if(QDir(QFileInfo(torrc).filePath()).isRelative())
    torrc = QCoreApplication::applicationDirPath() + "/" + torrc;
  return QDir::convertSeparators(torrc);
}

/** Sets the defaults torrc that will be used when starting Tor.
 * \param torrc The defaults torrc to use.
 */
void
TorSettings::setDefaultsTorrc(const QString &torrc)
{
  setValue(SETTING_DEFAULTS_TORRC, torrc);
}

/** Get the address or hostname used to connect to Tor */
QHostAddress
TorSettings::getControlAddress() const
{
  QString addr = defaultValue(SETTING_CONTROL_ADDR).toString();
  with_torrc_value(SETTING_CONTROL_ADDR) {
    addr = ret.at(0);
  }
  return QHostAddress(addr);
}

/** Set the address or hostname used to connect to Tor */
void
TorSettings::setControlAddress(const QHostAddress &addr)
{
  setVolatileValue(SETTING_CONTROL_ADDR, addr.toString());
}

/** Get the control port used to connect to Tor */
quint16
TorSettings::getControlPort() const
{
  quint16 port = defaultValue(SETTING_CONTROL_PORT).toInt();
  with_torrc_value(SETTING_CONTROL_PORT) {
    port = (quint16)ret.at(0).toUInt();
  }
  return port;
}

/** Get the socks port used to connect to Tor */
quint16
TorSettings::getSocksPort() const
{
  quint16 port = defaultValue(SETTING_SOCKS_PORT).toInt();
  with_torrc_value(SETTING_SOCKS_PORT) {
    port = (quint16)ret.at(0).toUInt();
  }
  return port;
}

/** Set the control port used to connect to Tor */
void
TorSettings::setControlPort(quint16 port)
{
  setVolatileValue(SETTING_CONTROL_PORT, port);
}

/** Set the socks port used to connect to Tor */
void
TorSettings::setSocksPort(quint16 port)
{
  setVolatileValue(SETTING_SOCKS_PORT, port);
}

/** Get the path for ControlSocket */
QString
TorSettings::getSocketPath() const
{
  with_torrc_value(SETTING_SOCKET_PATH) {
    return ret.at(0);
  }
  return defaultValue(SETTING_SOCKET_PATH).toString();
}

/** Set the path for ControlSocket */
void
TorSettings::setSocketPath(const QString &path)
{
  setVolatileValue(SETTING_SOCKET_PATH, path);
}

/** Get the current control method */
ControlMethod::Method
TorSettings::getControlMethod() const
{
  return ControlMethod::fromString(localValue(SETTING_CONTROL_METHOD).toString());
}

/** Set the control method */
void
TorSettings::setControlMethod(ControlMethod::Method method)
{
  setValue(SETTING_CONTROL_METHOD, ControlMethod::toString(method));
}

/** Returns the plaintext (i.e., not hashed) control password used when
 * authenticating to Tor. */
QString
TorSettings::getControlPassword() const
{
  return localValue(SETTING_CONTROL_PASSWORD).toString();
}

/** Sets the control password used when starting Tor with
 * HashedControlPassword to <b>password</b>. */
void
TorSettings::setControlPassword(const QString &password)
{
  setValue(SETTING_CONTROL_PASSWORD, password);
}

/** Returns true if a new, random control password is to be used each time Tor
 * is started. */
bool
TorSettings::useRandomPassword() const
{
  return localValue(SETTING_USE_RANDOM_PASSWORD).toBool();
}

/** Sets whether or not to generate and use a random control password each
 * time Tor is started. */
void
TorSettings::setUseRandomPassword(bool useRandomPassword)
{
  setValue(SETTING_USE_RANDOM_PASSWORD, useRandomPassword);
}

/** Returns the current authentication method used when connecting to Tor. */
TorSettings::AuthenticationMethod
TorSettings::getAuthenticationMethod() const
{
  AuthenticationMethod type = UnknownAuth;
  TorControl *tc = torControl();

  if (tc && tc->isConnected()) {
    QHash<QString,QString> conf;
    conf.insert(TOR_ARG_COOKIE_AUTH, "");
    conf.insert(TOR_ARG_HASHED_PASSWORD, "");
    if (tc->getConf(conf)) {
      if (conf.value(TOR_ARG_COOKIE_AUTH) == "1")
        type = CookieAuth;
      else if (!conf.value(TOR_ARG_HASHED_PASSWORD).isEmpty())
        type = PasswordAuth;
    }
  }
  if (type == UnknownAuth)
    type = toAuthenticationMethod(localValue(SETTING_AUTH_METHOD).toString());
  return (type == UnknownAuth ? DEFAULT_AUTH_METHOD : type);
}

/** Sets the authentication method used when starting Tor to <b>method</b>. */
void
TorSettings::setAuthenticationMethod(AuthenticationMethod method)
{
  setValue(SETTING_AUTH_METHOD, toString(method));
}

/** Returns the current list of ports that will cause Tor to issue a warning
 * when the user tries to connect to one of them. */
QList<quint16>
TorSettings::getWarnPlaintextPorts() const
{
  QList<quint16> out;
  QStringList ports;

  ports = Vidalia::torrc()->value(SETTING_WARN_PLAINTEXT_PORTS);
  if(ports.size() > 0)
    ports = ports.at(0).split(",");
  foreach (QString port, ports) {
    out << port.toUInt();
  }
  return out;
}

/** Sets the list of ports that will cause Tor to issue a warning when the
 * user tries to connect to one of them. */
void
TorSettings::setWarnPlaintextPorts(const QList<quint16> &ports)
{
  QStringList warnList;
  foreach (quint16 port, ports) {
    warnList << QString::number(port);
  }
  setValue(SETTING_WARN_PLAINTEXT_PORTS, warnList.join(","));
}

/** Returns the current list of ports that will cause Tor to reject the
 * connection when the user tries to connect to one of them. */
QList<quint16>
TorSettings::getRejectPlaintextPorts() const
{
  QList<quint16> out;
  QStringList ports;
  ports = Vidalia::torrc()->value(SETTING_REJECT_PLAINTEXT_PORTS);
  if(ports.size() > 0)
    ports = ports.at(0).split(",");
  foreach (QString port, ports) {
    out << port.toUInt();
  }
  return out;
}

/** Sets the list of ports that will cause Tor to reject the connection
 * when the user tries to connect to one of them. */
void
TorSettings::setRejectPlaintextPorts(const QList<quint16> &ports)
{
  QStringList rejectList;
  foreach (quint16 port, ports) {
    rejectList << QString::number(port);
  }
  setValue(SETTING_REJECT_PLAINTEXT_PORTS, rejectList.join(","));
}

/** Returns the string description of the authentication method specified by
 * <b>method</b>. The authentication method string is stored in Vidalia's
 * configuration file. */
QString
TorSettings::toString(AuthenticationMethod method) const
{
  switch (method) {
    case NullAuth:  return "none";
    case PasswordAuth:  return "password";
    case CookieAuth:  return "cookie";
    default: break;
  }
  return "unknown";
}

/** Returns the AuthenticationMethod enum value for the string
 * description of the authentication method given in <b>authMethod</b>. */
TorSettings::AuthenticationMethod
TorSettings::toAuthenticationMethod(const QString &authMethod) const
{
  QString str = authMethod.toLower();
  if (str == toString(NullAuth))
    return NullAuth;
  else if (str == toString(PasswordAuth))
    return PasswordAuth;
  else if (str == toString(CookieAuth))
    return CookieAuth;
  return UnknownAuth;
}

/** Generates a random control password consisting of PASSWORD_LEN characters. */
QString
TorSettings::randomPassword()
{
  return crypto_rand_string(PASSWORD_LEN);
}

/** Returns the hash of <b>password</b> as given by the command "tor
 * --hash-password foo". */
QString
TorSettings::hashPassword(const QString &password)
{
  QByteArray salt;

  /* Generate an 8 octet salt value. Bail if we fail to generate enough
   * random bytes (unlikely). */
  while (salt.size() < 8) {
    QByteArray bytes = crypto_rand_bytes(8-salt.size());
    if (bytes.isNull())
      return QString();
    salt.append(bytes);
  }

  /* Generate the salted hash of the specified password. 96 is the one-octet
   * RFC 2440 coded count value hardcoded into Tor. Specifies that we should
   * hash 64K worth of data. */
  QByteArray key = crypto_secret_to_key(password, salt, 96);
  if (key.isNull())
    return QString();
  salt.append(96); /* Append the coded count value to the salt */

  /* Convert the result to hexadecimal and put it in the format Tor wants. */
  return QString("16:%1%2").arg(base16_encode(salt))
                           .arg(base16_encode(key));
}

void
TorSettings::setBootstrap(bool enabled)
{
  setValue(SETTING_BOOTSTRAP, enabled);
}

bool
TorSettings::bootstrap() const
{
  return value(SETTING_BOOTSTRAP).toBool();
}

void
TorSettings::setBootstrapFrom(const QString &from)
{
  setValue(SETTING_BOOTSTRAP_FROM, from);
}

QString
TorSettings::bootstrapFrom() const
{
  return QDir::convertSeparators(value(SETTING_BOOTSTRAP_FROM).toString());
}

bool
TorSettings::autoControlPort() const
{
  return getControlPort() == 0;
}

void
TorSettings::setAutoControlPort(const bool autoControl)
{
  setVolatileValue(SETTING_AUTOCONTROL, autoControl);
}
