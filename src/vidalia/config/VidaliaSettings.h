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

  /** Returns true if Vidalia should start a proxy application when it
   * starts. */
  bool runProxyAtStart();
  /** Set whether to run a proxy application when Vidalia starts. */
  void setRunProxyAtStart(bool run);

  /** Returns a fully-qualified path to the proxy server, including the
   * executable name. */
  QString getProxyExecutable() const;
  /** Sets the location and name of the proxy server executable to the given
   * string. If set to the empty string, the proxy will not be started. */
  void setProxyExecutable(const QString &proxyExecutable);

  /** Returns a list containing additional command line arguments to be
   * passed to ProxyExecutable */
  QString getProxyExecutableArguments() const;
  /** Sets the additional arguments to be passed to Proxy Executable */
  void setProxyExecutableArguments(const QString &proxyExecutableArguments);
  
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
};

#endif

