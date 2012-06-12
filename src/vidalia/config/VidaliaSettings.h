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
** \file VidaliaSettings.h
** \brief General Vidalia settings, such as language and interface style
*/

#ifndef _VIDALIASETTINGS_H
#define _VIDALIASETTINGS_H

#include "VSettings.h"

#include <QDateTime>

/** Public setting keys */
#define SETTING_REMEMBER_SHUTDOWN      "RememberShutdown"
#define SETTING_REMEMBER_DONTWARNEXIT  "DontWarnExitNodes"
#define SETTING_REMEMBER_DONTWARNENTRY "DontWarnEntryNodes"

/** Handles saving and restoring Vidalia's settings, such as the
 * location of Tor, the control port, etc.
 *
 * NOTE: Qt 4.1 documentation states that constructing a QSettings object is
 * "very fast", so we shouldn't need to create a global instance of this
 * class.
 */
class VidaliaSettings : public VSettings
{
  Q_OBJECT

public:
  enum IconPosition {
    Tray,
    Dock,
    Both,
  };

  /** Default constructor. */
  VidaliaSettings();

  /** Gets the currently preferred language code for Vidalia. */
  QString getLanguageCode();
  /** Saves the preferred language code. */
  void setLanguageCode(QString languageCode);

  /** Gets the interface style key (e.g., "windows", "motif", etc.) */
  QString getInterfaceStyle();
  /** Sets the interface style key. */
  void setInterfaceStyle(QString styleKey);

  /** Returns true if Vidalia should start Tor when it starts. */
  bool runTorAtStart();
  /** Set whether to run Tor when Vidalia starts. */
  void setRunTorAtStart(bool run);

  /** Returns true if Vidalia's main window should be visible when the
   * application starts. */
  bool showMainWindowAtStart();
  /** Sets whether to show Vidalia's main window when the application starts. */
  void setShowMainWindowAtStart(bool show);

  /** Returns true if Vidalia should start on system boot. */
  bool runVidaliaOnBoot();
  /** Set whether to run Vidalia on system boot. */
  void setRunVidaliaOnBoot(bool run);

  /** If browserDirectory is empty, returns a fully-qualified path to
   * the web browser, including the executable name. If browserDirectory
   * is set, then returns the basename of the configured web browser */
  QString getBrowserExecutable() const;
  /** Sets the location and name of the web browser executable to the given
   * string. If set to the empty string, the browser will not be started. */
  void setBrowserExecutable(const QString &browserExecutable);

  /** Returns a fully-qualified path to the web browser directory */
  QString getBrowserDirectory() const;
  /** Sets the location and name of the web browser directory to the given string.
   * If set to the empty string, the browser will not be started. */
  void setBrowserDirectory(const QString &browserDirectory);

  /** Returns a fully-qualified path to the IM client, including the
   * executable name. */
  QString getIMExecutable() const;
  /** Sets the location and name of the IM client executable to the given
   * string. If set to the empty string, the client will not be started. */
  void setIMExecutable(const QString &IMExecutable);

  /** Returns true if Vidalia should automatically check for software updates.
   */
  bool isAutoUpdateEnabled() const;
  /** Sets to <b>enabled</b> whether Vidalia should automatically check for
   * software updates or not. */
  void setAutoUpdateEnabled(bool enabled);

  /** Returns the time at which Vidalia last checked for software updates. */
  QDateTime lastCheckedForUpdates() const;
  /** Sets to <b>checkedAt</b> the time at which Vidalia last checked for
   * available software updates. */
  void setLastCheckedForUpdates(const QDateTime &checkedAt);

  /** Returns true if Vidalia is currently configured to use a local GeoIP
   * database. */
  bool useLocalGeoIpDatabase() const;
  /** Enables or disables use of a local GeoIP database. */
  void setUseLocalGeoIpDatabase(bool enabled);

  /** Returns the file, if set, to be used as the local GeoIP database. */
  QString localGeoIpDatabase() const;
  /** Sets the file to use as a local GeoIP database. */
  void setLocalGeoIpDatabase(const QString &databaseFile);

  /** Returns the path where the plugins live */
  QString pluginPath() const;
  /** Sets the path where the plugins live */
  void setPluginPath(const QString &path);

  /** Get the icon preference */
  IconPosition getIconPref();

  /** Set the icon preference */
  void setIconPref(const IconPosition iconPref);
  QString toString(const IconPosition iconPref);
  IconPosition fromString(QString iconPref);

  /** Returns true if Vidalia should skip the version check for tor */
  bool skipVersionCheck() const;

  /** Returns true if Vidalia should not ask if the user wants to shutdown
   *  tor gracefully */
  bool rememberShutdown();
  /** Sets RememberShutdown to val */
  void setRememberShutdown(bool val);

  /** Returns true if AllowPanic is enabled */
  bool allowPanic() const;
  /** Sets AllowPanic to val */
  void setAllowPanic(bool val);

  /** Returns the path that should be eliminated for panic */
  QString panicPath() const;
  /** Sets the path for panic */
  void setPanicPath(const QString &path);

  /** Returns true if it's Vidalia's first run */
  bool firstRun() const;
  /** Sets Vidalia's first run option */
  void setFirstRun(bool val);

  /** Returns true if Vidalia should warn the user when setting Exit
   *  nodes by hand */
  bool dontWarnExitNodes() const;
  /** Sets wether Vidalia should warn the user when setting Exit nodes
   *  by hand */
  void setDontWarnExitNodes(bool val);

  /** Returns true if Vidalia should warn the user when setting Entry
   *  nodes by hand */
  bool dontWarnEntryNodes() const;
  /** Sets wether Vidalia should warn the user when setting Entry nodes
   *  by hand */
  void setDontWarnEntryNodes(bool val);
};

#endif

