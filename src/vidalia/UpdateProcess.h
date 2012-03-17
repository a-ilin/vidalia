/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#ifndef _UPDATEPROCESS_H
#define _UPDATEPROCESS_H

#include "PackageInfo.h"

#include <QProcess>
#include <QDateTime>
#include <QStringList>
#include <QUrl>


class UpdateProcess : public QProcess
{
  Q_OBJECT

public:
  enum BundleInfo {
    TorBundleInfo,
  };

  /** Default constructor.
   */
  UpdateProcess(QObject *parent = 0);

  /** Begin a check for software updates that may be available for the
   * software package specified by <b>bi</b>.
   */
  void checkForUpdates(BundleInfo bi);

  /** Instructs the software update process to install previously downloaded
   * files for <b>bi</b>.
   */
  void installUpdates(BundleInfo bi);

  /** Returns true if the update process is currently in the middle of an
   * operation, such as checking for or installing updates.
   */
  bool isRunning() const;

  /** Sets the port to use as a SOCKS proxy to <b>port</b>. If <b>port</b> is
   * set to 0, then no SOCKS proxy will be used when checking for updates.
   */
  void setSocksPort(quint16 port);

  /** Return the time at which we should next check for available updates,
   * given the last we checked was at <b>lastCheckedAt</b>.
   */
  static QDateTime nextCheckForUpdates(const QDateTime &lastCheckedAt);

  /** Return true if we should check for available software udpates, given
   * the last time we checked was at <b>lastCheckedAt</b>. The returned
   * QDateTime will be in UTC.
   */
  static bool shouldCheckForUpdates(const QDateTime &lastCheckedAt);

  /** Returns the preferred interval (in seconds) between executions of the
   * Glider process to check for available software updates.
   */
  static int checkForUpdatesInterval();

  /** Returns the path and filename of the software update executable.
   */
  static QString updateExecutable();

  /** Returns the path in which the software update executable should write
   * all of its state information.
   */
  static QString updateRepositoryDir();

signals:
  /** Emitted when the check for available software updates failed.
   * <b>errmsg</b> contains a human-readable description of the problem
   * encountered.
   */
  void checkForUpdatesFailed(QString errmsg);

  /** Emitted while an updated package download is in progress. <b>url</b> is
   * location of the update, <b>bytesReceived</b> is how many bytes have been
   * downloaded so far and <b>bytesTotal</b> is the total size of the package
   * being downloaded. */
  void downloadProgress(QString url, int bytesReceived, int bytesTotal);

  /** Emitted when updated software packages in bundle <b>bi</b> are
   * are available. <b>packages</b> contains a collection of PackageInfo objects
   * describing the updates available for installation.
   */
  void updatesAvailable(UpdateProcess::BundleInfo bi, PackageList packages);

  /** Emitted after all available updated packages have been successfully
   * installed.
   */
  void updatesInstalled(int nPackagesInstalled);

  /** Emitted when there is an error installing one or more updated software
   * packages. <b>errmsg</b> might even contain a useful description of the
   * error encountered (but don't bet the farm on it).
   */
  void installUpdatesFailed(QString errmsg);

public slots:
    /** Cancels the currently running software update operation immediately. */
  void cancel();

protected slots:
  /** Called when there is data to be read from the update process's stdout.
   * Reads and parses all available data.
   */
  void readStandardOutput();

  /** Called when there is data to be read from the update process's stderr.
   * Reads and parses all available data.
   */
  void readStandardError();

  /** Called when the underlying QProcess encounters an error.
   */
  void onError(QProcess::ProcessError error);

  /** Called when the auto-update process has terminated.
   */
  void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

protected:
  enum UpdateCommand {
    NoCommand,
    CheckForUpdates,
    InstallUpdates,
  };

  /** Converts a BundleInfo enum value to its proper Thandy-recognized URL
   * for the current OS and architecture. */
  QString bundleInfoToString(BundleInfo bundleInfo);

  /** Returns a PackageInfo object containing information about the updated
   * package specified by the /pkginfo/ URL in <b>package</b>.
   */
  static PackageInfo packageInfo(const QString &package);

  /** Returns a PackageInfo object populated with information extracted
   * from a Thandy-formatted XML document given by <b>xml</b>.
   */
  static PackageInfo packageInfoFromXml(const QByteArray &xml);

private:
  /** Enum value of the current auto-update operation. */
  UpdateCommand _currentCommand;

  /** Enum value of the last bundle for which we performed some action
   * (e.g., check for updates, install an update, etc. */
  BundleInfo _currentBundle;

  /** List of packages that have available updates. */
  PackageList _packageList;

  /** Currently configured SOCKS port. */
  quint16 _socksPort;
};

#endif

