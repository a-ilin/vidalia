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
 * \file vidaliasettings.cpp
 * \version $Id$
 * \brief General Vidalia settings, such as language and interface style
 */

#include <QDir>
#include <QCoreApplication>
#include <QStyleFactory>
#include <languagesupport.h>
#include <vidalia.h>

#include "vidaliasettings.h"

#if defined(Q_WS_WIN)
#include <win32.h>
#endif

#define SETTING_LANGUAGE            "LanguageCode"
#define SETTING_STYLE               "InterfaceStyle"
#define SETTING_RUN_TOR_AT_START    "RunTorAtStart"
#define SETTING_DATA_DIRECTORY      "DataDirectory"
#define SETTING_SHOW_MAINWINDOW_AT_START  "ShowMainWindowAtStart"
#define SETTING_BROWSER_EXECUTABLE  "BrowserExecutable"
#define SETTING_PROXY_EXECUTABLE  "ProxyExecutable"
#define SETTING_PROXY_EXECUTABLE_ARGUMENTS  "ProxyExecutableArguments"

#if defined(Q_OS_WIN32)
#define STARTUP_REG_KEY        "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define VIDALIA_REG_KEY        "Vidalia" 
#endif


/** Default Constructor */
VidaliaSettings::VidaliaSettings()
{
#if defined(Q_WS_MAC)
  setDefault(SETTING_STYLE, "macintosh (aqua)");
#else
  static QStringList styles = QStyleFactory::keys();
#if defined(Q_WS_WIN)
  if (styles.contains("windowsvista", Qt::CaseInsensitive))
    setDefault(SETTING_STYLE, "windowsvista");
  else
#endif
  {
    if (styles.contains("cleanlooks", Qt::CaseInsensitive))
      setDefault(SETTING_STYLE, "cleanlooks");
    else
      setDefault(SETTING_STYLE, "plastique");
  }
#endif

  setDefault(SETTING_LANGUAGE, LanguageSupport::defaultLanguageCode());
  setDefault(SETTING_RUN_TOR_AT_START, true);
  setDefault(SETTING_SHOW_MAINWINDOW_AT_START, true);
  setDefault(SETTING_BROWSER_EXECUTABLE, "");
  setDefault(SETTING_PROXY_EXECUTABLE, "");
  setDefault(SETTING_PROXY_EXECUTABLE_ARGUMENTS, QStringList());
}

/** Gets the currently preferred language code for Vidalia. */
QString
VidaliaSettings::getLanguageCode()
{
  return value(SETTING_LANGUAGE).toString();
}

/** Sets the preferred language code. */
void
VidaliaSettings::setLanguageCode(QString languageCode)
{
  setValue(SETTING_LANGUAGE, languageCode);
}

/** Gets the interface style key (e.g., "windows", "motif", etc.) */
QString
VidaliaSettings::getInterfaceStyle()
{
  return value(SETTING_STYLE).toString();
}

/** Sets the interface style key. */
void
VidaliaSettings::setInterfaceStyle(QString styleKey)
{
  setValue(SETTING_STYLE, styleKey);
}

/** Returns true if Tor is to be run when Vidalia starts. */
bool
VidaliaSettings::runTorAtStart()
{
  return value(SETTING_RUN_TOR_AT_START).toBool();
}

/** If <b>run</b> is set to true, then Tor will be run when Vidalia starts. */
void
VidaliaSettings::setRunTorAtStart(bool run)
{
  setValue(SETTING_RUN_TOR_AT_START, run);
}

/** Returns true if Vidalia's main window should be visible when the
 * application starts. */
bool
VidaliaSettings::showMainWindowAtStart()
{
  return value(SETTING_SHOW_MAINWINDOW_AT_START).toBool();
}

/** Sets whether to show Vidalia's main window when the application starts. */
void
VidaliaSettings::setShowMainWindowAtStart(bool show)
{
  setValue(SETTING_SHOW_MAINWINDOW_AT_START, show);
}


/** Returns true if Vidalia is set to run on system boot. */
bool
VidaliaSettings::runVidaliaOnBoot()
{
#if defined(Q_WS_WIN)
  if (!win32_registry_get_key_value(STARTUP_REG_KEY, VIDALIA_REG_KEY).isEmpty()) {
    return true;
  } else {
    return false;
  }
#else
  /* Platforms other than windows aren't supported yet */
  return false;
#endif
}

/** If <b>run</b> is set to true, then Vidalia will run on system boot. */
void
VidaliaSettings::setRunVidaliaOnBoot(bool run)
{
#if defined(Q_WS_WIN)
  if (run) {
    win32_registry_set_key_value(STARTUP_REG_KEY, VIDALIA_REG_KEY,
        QString("\"" +
                QDir::convertSeparators(QCoreApplication::applicationFilePath())) +
                "\"");
  } else {
    win32_registry_remove_key(STARTUP_REG_KEY, VIDALIA_REG_KEY);
  }
#else
  /* Platforms othe rthan windows aren't supported yet */
  Q_UNUSED(run);
  return;
#endif
}

/** Returns a fully-qualified path to the web browser, including the
 * executable name. */
QString
VidaliaSettings::getBrowserExecutable() const
{
  return QDir::convertSeparators(value(SETTING_BROWSER_EXECUTABLE).toString());
}

/** Sets the location and name of the web browser executable to the given string.
 * If set to the empty string, the browser will not be started. */
void
VidaliaSettings::setBrowserExecutable(const QString &browserExecutable)
{
  setValue(SETTING_BROWSER_EXECUTABLE, browserExecutable);
}

/** Returns a fully-qualified path to the proxy server, including the
 * executable name. */
QString
VidaliaSettings::getProxyExecutable() const
{
  return QDir::convertSeparators(value(SETTING_PROXY_EXECUTABLE).toString());
}

/** Sets the location and name of the proxy server executable to the given
 * string. If set to the empty string, the proxy will not be started. */
void
VidaliaSettings::setProxyExecutable(const QString &proxyExecutable)
{
  setValue(SETTING_PROXY_EXECUTABLE, proxyExecutable);
}

/** Returns a list containing additional command line arguments to be passed
 * to ProxyExecutable */
QStringList
VidaliaSettings::getProxyExecutableArguments() const
{
  return value(SETTING_PROXY_EXECUTABLE_ARGUMENTS).toStringList();
}

/** Sets the additional arguments to be passed to Proxy Executable */
void
VidaliaSettings::setProxyExecutableArguments(const QStringList
                                             &proxyExecutableArguments)
{
  setValue(SETTING_PROXY_EXECUTABLE_ARGUMENTS, proxyExecutableArguments);
}

