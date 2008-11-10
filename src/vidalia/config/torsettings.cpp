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
** \file torsettings.cpp
** \version $Id$
** \brief Settings used for starting and running Tor
*/

#include <QDir>
#include <QProcess>
#include <crypto.h>
#include <vidalia.h>

#if defined(Q_OS_WIN32)
#include <QFileInfo>
#include <win32.h>
#endif

#include "torsettings.h"

/* Tor Settings */
#define SETTING_TOR_EXECUTABLE      "TorExecutable"
#define SETTING_TORRC               "Torrc"
#define SETTING_CONTROL_ADDR        "ControlAddr"
#define SETTING_CONTROL_PORT        "ControlPort"
#define SETTING_AUTH_TOKEN          "AuthToken"
#define SETTING_TOR_USER            "User"
#define SETTING_TOR_GROUP           "Group"
#define SETTING_DATA_DIRECTORY      "DataDirectory"
#define SETTING_AUTH_METHOD         "AuthenticationMethod"
#define SETTING_CONTROL_PASSWORD    "ControlPassword"
#define SETTING_USE_RANDOM_PASSWORD "UseRandomPassword"

/** Default to using hashed password authentication */
#define DEFAULT_AUTH_METHOD     PasswordAuth

/* Arguments we can pass to Tor on the command-line */
#define TOR_ARG_CONTROL_PORT    "ControlPort"
#define TOR_ARG_TORRC           "-f"
#define TOR_ARG_DATA_DIRECTORY  "DataDirectory"
#define TOR_ARG_HASHED_PASSWORD "HashedControlPassword"
#define TOR_ARG_COOKIE_AUTH     "CookieAuthentication"

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
  setDefault(SETTING_CONTROL_ADDR,  "127.0.0.1");
  setDefault(SETTING_CONTROL_PORT,  9051);
  setDefault(SETTING_AUTH_METHOD,   toString(DEFAULT_AUTH_METHOD));
  setDefault(SETTING_DATA_DIRECTORY, "");
  setDefault(SETTING_CONTROL_PASSWORD, "");
  setDefault(SETTING_USE_RANDOM_PASSWORD, true);
}

/** Applies any changes to Tor's control port or authentication settings. */
bool
TorSettings::apply(QString *errmsg)
{
  QHash<QString, QString> conf;
  QString hashedPassword;

  conf.insert(SETTING_CONTROL_PORT,
              localValue(SETTING_CONTROL_PORT).toString());
  
  AuthenticationMethod authMethod = 
    toAuthenticationMethod(localValue(SETTING_AUTH_METHOD).toString());
  switch (authMethod) {
    case CookieAuth:
      conf.insert(TOR_ARG_COOKIE_AUTH,    "1");
      conf.insert(TOR_ARG_HASHED_PASSWORD, "");
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
      conf.insert(TOR_ARG_COOKIE_AUTH,    "0");
      conf.insert(TOR_ARG_HASHED_PASSWORD, hashedPassword);
      break;
    default:
      conf.insert(TOR_ARG_COOKIE_AUTH,    "0");
      conf.insert(TOR_ARG_HASHED_PASSWORD, "");
  }
  return torControl()->setConf(conf, errmsg);
}

/** Gets the location of Tor's data directory. */
QString
TorSettings::getDataDirectory() const
{
  return QDir::convertSeparators(value(SETTING_DATA_DIRECTORY).toString());
}

/** Sets the location to use as Tor's data directory. */
void
TorSettings::setDataDirectory(const QString &dataDirectory)
{
  setValue(SETTING_DATA_DIRECTORY, dataDirectory);
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
  return QDir::convertSeparators(localValue(SETTING_TORRC).toString());
}

/** Sets the torrc that will be used when starting Tor.
 * \param torrc The torrc to use. 
 */
void
TorSettings::setTorrc(const QString &torrc)
{
  setValue(SETTING_TORRC, torrc);
}

/** Get the address or hostname used to connect to Tor */
QHostAddress
TorSettings::getControlAddress() const
{
  QString addr = localValue(SETTING_CONTROL_ADDR).toString();
  return QHostAddress(addr);
}

/** Set the address or hostname used to connect to Tor */
void
TorSettings::setControlAddress(const QHostAddress &addr)
{
  setValue(SETTING_CONTROL_ADDR, addr.toString());
}

/** Get the control port used to connect to Tor */
quint16
TorSettings::getControlPort() const
{
  return (quint16)value(SETTING_CONTROL_PORT).toInt();
}

/** Set the control port used to connect to Tor */
void
TorSettings::setControlPort(quint16 port)
{
  setValue(SETTING_CONTROL_PORT, port);
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
  TorSettings settings;
  QProcess tor;
  QString dataDirectory, line;
  QStringList args;

  /* Tor writes its state file even if all we're doing is --hash-password. So
   * if the user has configured a non-default data directory, then include
   * that in the list of command line arguments. */
  dataDirectory = settings.getDataDirectory();
  if (!dataDirectory.isEmpty())
    args << "DataDirectory" << dataDirectory;
  args << "--hash-password" << password;
  
  /* Run Tor, tell it to hash the given password, and then wait for it to
   * finish. */
  tor.start(settings.getExecutable(), args);
  if (!tor.waitForStarted() || !tor.waitForFinished())
    return QString();

  /* The hashed password will (hopefully) be the line that starts with "16:" */
  while (tor.canReadLine()) {
    line = tor.readLine();
    if (line.startsWith("16:"))
      return line.trimmed();
  }
  return QString();
}

