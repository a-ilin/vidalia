/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#include <QtDebug>

#include "gliderprocess.h"


GliderProcess::GliderProcess(QObject *parent)
  : QProcess(parent)
{  
  connect(this, SIGNAL(readyReadStandardError()),
          this, SLOT(readStandardError()));
  connect(this, SIGNAL(readyReadStandardOutput()),
          this, SLOT(readStandardOutput()));
}

void
GliderProcess::checkForUpdates(const QString &gliderExecutable,
                               const QStringList &args)
{
  start(gliderExecutable, args);
}

void
GliderProcess::readStandardError()
{
  QString line;

  setReadChannel(QProcess::StandardError);
  while (canReadLine()) {
    line = readLine();
    qDebug() << QString("updater (stderr): %1").arg(line);
  }  
}

void
GliderProcess::readStandardOutput()
{
  QString line;

  setReadChannel(QProcess::StandardOutput);
  while (canReadLine()) {
    line = readLine();
    qDebug() << QString("updater (stdout): %1").arg(line);
  }
}

int
GliderProcess::checkForUpdatesInterval()
{
  /* XXX: Check once a day. I totally made this up. We want to do something
   * smarter here. */
  return 24*60*60;
}

QDateTime
GliderProcess::nextCheckForUpdates(const QDateTime &lastCheckedAt)
{
  return lastCheckedAt.toUTC().addSecs(checkForUpdatesInterval());
}

bool
GliderProcess::shouldCheckForUpdates(const QDateTime &lastCheckedAt)
{
  QDateTime nextCheck = nextCheckForUpdates(lastCheckedAt);
  return (nextCheck >= QDateTime::currentDateTime().toUTC());
}

