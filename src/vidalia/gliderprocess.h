/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#ifndef _GLIDERPROCESS_H
#define _GLIDERPROCESS_H

#include <QProcess>
#include <QDateTime>
#include <QStringList>


class GliderProcess : public QProcess
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  GliderProcess(QObject *parent = 0);

  /** Begin a check for software updates using the Glider binary specified
   * by <b>gliderExecutable</b>. The arguments in <b>args</b> will be given
   * to Glider on the command line.
   */
  void checkForUpdates(const QString &gliderExecutable,
                       const QStringList &args);

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
   * Glider process to check for available software updates. */
  static int checkForUpdatesInterval();

protected slots:
  /** Called when there is data to be read from the update process's stdout.
   * Reads and parses all available data.
   */
  void readStandardOutput();

  /** Called when there is data to be read from the update process's stderr.
   * Reads and parses all available data.
   */
  void readStandardError();
};

#endif

