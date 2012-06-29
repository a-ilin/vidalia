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
** \file VidaliaSettings.cpp
** \brief General Vidalia settings, such as language and interface style
*/

#include "VidaliaSettings.h"
#include "LanguageSupport.h"
#include "Vidalia.h"
#if defined(Q_WS_WIN)
#include "win32.h"
#endif

#include <QDir>
#include <QCoreApplication>
#include <QStyleFactory>

#define SETTING_LANGUAGE            "LanguageCode"
#define SETTING_STYLE               "InterfaceStyle"
#define SETTING_RUN_TOR_AT_START    "RunTorAtStart"
#define SETTING_DATA_DIRECTORY      "DataDirectory"
#define SETTING_SHOW_MAINWINDOW_AT_START  "ShowMainWindowAtStart"
#define SETTING_BROWSER_EXECUTABLE  "BrowserExecutable"
#define SETTING_BROWSER_DIRECTORY   "BrowserDirectory"
#define SETTING_IM_EXECUTABLE       "IMExecutable"
#define SETTING_CHECK_FOR_UPDATES   "CheckForUpdates"
#define SETTING_LAST_UPDATE_CHECK   "LastUpdateCheck"
#define SETTING_USE_LOCAL_GEOIP_DATABASE  "UseLocalGeoIpDatabase"
#define SETTING_LOCAL_GEOIP_DATABASE "LocalGeoIpDatabase"
#define SETTING_PLUGIN_PATH         "PluginPath"
#define SETTING_SKIP_VERSION_CHECK  "SkipVersionCheck"
#define SETTING_ALLOW_PANIC         "AllowPanic"
#define SETTING_PANIC_PATH          "PanicPath"
#define SETTING_FIRST_RUN           "FirstRun"
#define SETTING_VIDALIA_VERSION     "VidaliaVersion"

#if defined(Q_OS_WIN32)
#define STARTUP_REG_KEY        "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define VIDALIA_REG_KEY        "Vidalia"
#endif

#define SETTING_ICON_PREF  "IconPref"

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
  setDefault(SETTING_IM_EXECUTABLE, "");
#if defined(Q_WS_WIN)
  setDefault(SETTING_CHECK_FOR_UPDATES, true);
#else
  setDefault(SETTING_CHECK_FOR_UPDATES, false);
#endif
  setDefault(SETTING_LAST_UPDATE_CHECK, QDateTime());
  setDefault(SETTING_USE_LOCAL_GEOIP_DATABASE, false);
  setDefault(SETTING_LOCAL_GEOIP_DATABASE, "");

  setDefault(SETTING_PLUGIN_PATH, vApp->dataDirectory());
  setDefault(SETTING_ICON_PREF, Both);
  setDefault(SETTING_SKIP_VERSION_CHECK, false);
  setDefault(SETTING_REMEMBER_SHUTDOWN, false);
  setDefault(SETTING_ALLOW_PANIC, false);
  setDefault(SETTING_PANIC_PATH, "");
  setDefault(SETTING_FIRST_RUN, true);
  setDefault(SETTING_REMEMBER_DONTWARNEXIT, true);

  setValue(SETTING_VIDALIA_VERSION, Vidalia::version());
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

/** If browserDirectory is empty, returns a fully-qualified path to
 * the web browser, including the executable name. If browserDirectory
 * is set, then returns the basename of the configured web browser */
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

/** Returns a fully-qualified path to the web browser directory */
QString
VidaliaSettings::getBrowserDirectory() const
{
  return QDir::convertSeparators(value(SETTING_BROWSER_DIRECTORY).toString());
}

/** Sets the location and name of the web browser directory to the given string.
 * If set to the empty string, the browser will not be started. */
void
VidaliaSettings::setBrowserDirectory(const QString &browserDirectory)
{
  setValue(SETTING_BROWSER_DIRECTORY, browserDirectory);
}

/** Returns a fully-qualified path to the IM client, including the
 * executable name. */
QString
VidaliaSettings::getIMExecutable() const
{
  return QDir::convertSeparators(value(SETTING_IM_EXECUTABLE).toString());
}

/** Sets the location and name of the IM client executable to the given string.
 * If set to the empty string, the client will not be started. */
void
VidaliaSettings::setIMExecutable(const QString &IMExecutable)
{
  setValue(SETTING_IM_EXECUTABLE, IMExecutable);
}

bool
VidaliaSettings::isAutoUpdateEnabled() const
{
  return value(SETTING_CHECK_FOR_UPDATES).toBool();
}

void
VidaliaSettings::setAutoUpdateEnabled(bool enabled)
{
  setValue(SETTING_CHECK_FOR_UPDATES, enabled);
}

QDateTime
VidaliaSettings::lastCheckedForUpdates() const
{
  return value(SETTING_LAST_UPDATE_CHECK).toDateTime();
}

void
VidaliaSettings::setLastCheckedForUpdates(const QDateTime &checkedAt)
{
  setValue(SETTING_LAST_UPDATE_CHECK, checkedAt);
}

bool
VidaliaSettings::useLocalGeoIpDatabase() const
{
  return value(SETTING_USE_LOCAL_GEOIP_DATABASE).toBool();
}

void
VidaliaSettings::setUseLocalGeoIpDatabase(bool enabled)
{
  setValue(SETTING_USE_LOCAL_GEOIP_DATABASE, enabled);
}

QString
VidaliaSettings::localGeoIpDatabase() const
{
  return QDir::convertSeparators(value(SETTING_LOCAL_GEOIP_DATABASE).toString());
}

void
VidaliaSettings::setLocalGeoIpDatabase(const QString &databaseFile)
{
  setValue(SETTING_LOCAL_GEOIP_DATABASE, databaseFile);
}

QString
VidaliaSettings::pluginPath() const
{
  return QDir::convertSeparators(value(SETTING_PLUGIN_PATH).toString());
}

void
VidaliaSettings::setPluginPath(const QString &path)
{
  setValue(SETTING_PLUGIN_PATH, path);
}

/** Get the icon preference */
VidaliaSettings::IconPosition
VidaliaSettings::getIconPref()
{
  return fromString(value(SETTING_ICON_PREF).toString());
}

/** Set the icon preference */
void
VidaliaSettings::setIconPref(const IconPosition iconPref)
{
  setValue(SETTING_ICON_PREF, toString(iconPref));
}

QString
VidaliaSettings::toString(const IconPosition iconPref)
{
  switch(iconPref) {
  case Dock: return "Dock";
  case Tray: return "Tray";
  default: return "Both";
  }
}

VidaliaSettings::IconPosition
VidaliaSettings::fromString(QString iconPref)
{
  if(iconPref == "Dock") return Dock;
  if(iconPref == "Tray") return Tray;

  return Both;
}

bool
VidaliaSettings::skipVersionCheck() const
{
  return value(SETTING_SKIP_VERSION_CHECK).toBool();
}

bool
VidaliaSettings::rememberShutdown()
{
  return value(SETTING_REMEMBER_SHUTDOWN).toBool();
}

void
VidaliaSettings::setRememberShutdown(bool val)
{
  setValue(SETTING_REMEMBER_SHUTDOWN, val);
}

bool
VidaliaSettings::allowPanic() const
{
  return value(SETTING_ALLOW_PANIC).toBool();
}

void
VidaliaSettings::setAllowPanic(bool val)
{
  setValue(SETTING_ALLOW_PANIC, val);
}

QString
VidaliaSettings::panicPath() const
{
  return value(SETTING_PANIC_PATH).toString();
}

void
VidaliaSettings::setPanicPath(const QString &path)
{
  setValue(SETTING_PANIC_PATH, path);
}

bool
VidaliaSettings::firstRun() const
{
  return value(SETTING_FIRST_RUN).toBool();
}

void
VidaliaSettings::setFirstRun(bool val)
{
  setValue(SETTING_FIRST_RUN, val);
}

bool
VidaliaSettings::dontWarnExitNodes() const
{
  return value(SETTING_REMEMBER_DONTWARNEXIT).toBool();
}

void
VidaliaSettings::setDontWarnExitNodes(bool val)
{
  setValue(SETTING_REMEMBER_DONTWARNEXIT, val);
}

bool
VidaliaSettings::dontWarnEntryNodes() const
{
  return value(SETTING_REMEMBER_DONTWARNENTRY).toBool();
}

void
VidaliaSettings::setDontWarnEntryNodes(bool val)
{
  setValue(SETTING_REMEMBER_DONTWARNENTRY, val);
}
