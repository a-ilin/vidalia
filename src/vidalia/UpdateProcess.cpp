/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#include "UpdateProcess.h"
#include "Vidalia.h"

#include "stringutil.h"

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>


UpdateProcess::UpdateProcess(QObject *parent)
  : QProcess(parent)
{
  _currentCommand = NoCommand;
  _socksPort = 0;

  connect(this, SIGNAL(readyReadStandardError()),
          this, SLOT(readStandardError()));
  connect(this, SIGNAL(readyReadStandardOutput()),
          this, SLOT(readStandardOutput()));
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(onFinished(int, QProcess::ExitStatus)));

  setEnvironment(systemEnvironment());
}

void
UpdateProcess::checkForUpdates(BundleInfo bi)
{
  QStringList args;

  args << "update"  << "--force-check"
       << " --controller-log-format"
       << "--repo=" + updateRepositoryDir()
       << "--debug";
  if (_socksPort)
    args << "--socks-port=" + QString::number(_socksPort);

  args << bundleInfoToString(bi);

  vNotice("updater: launching auto-update executable: %1 %2")
                                           .arg(updateExecutable())
                                           .arg(args.join(" "));
  _currentBundle = bi;
  _currentCommand = CheckForUpdates;
  start(updateExecutable(), args);
}

void
UpdateProcess::installUpdates(BundleInfo bi)
{
  QStringList args;

  args << "update" << "--controller-log-format"
       << "--repo=" + updateRepositoryDir()
       << "--install";
  if (_socksPort)
    args << "--socks-port=" + QString::number(_socksPort);

  args << bundleInfoToString(bi);

  vNotice("updater: launching auto-update executable: %1 %2")
                                           .arg(updateExecutable())
                                           .arg(args.join(" "));
  _currentBundle = bi;
  _currentCommand = InstallUpdates;
  start(updateExecutable(), args);
}

void
UpdateProcess::setSocksPort(quint16 port)
{
  _socksPort = port;
}

bool
UpdateProcess::isRunning() const
{
  return (state() != QProcess::NotRunning);
}

void
UpdateProcess::cancel()
{
  if (_currentCommand == CheckForUpdates) {
#if defined(Q_OS_WIN32)
    kill();
#else
    terminate();
#endif
  }
}

void
UpdateProcess::readStandardError()
{
  int idx;
  bool ok;
  QString line, type;
  QHash<QString,QString> args;

  setReadChannel(QProcess::StandardError);
  while (canReadLine()) {
    line = readLine().trimmed();
    vInfo("updater (stderr): %1").arg(line);

    idx = line.indexOf(" ");
    if (idx < 0 || idx == line.length()-1)
      continue;
    type = line.mid(0, idx);
    line = line.mid(idx + 1);

    args = string_parse_keyvals(line, &ok);
    if (! ok)
      continue;
    else if (line.startsWith("thandy.InstallFailed: ", Qt::CaseInsensitive)) {
      /** XXX: This is a fucking kludge. If installation fails, Thandy just
       *       dumps a Python traceback that (for obvious reasons) doesn't
       *       follow the expected format. There isn't a defined control
       *       message type for this yet we'd really like the error, so treat
       *       this one specially.
       */
      emit installUpdatesFailed(line);
      continue;
    }

    if (! type.compare("CAN_INSTALL", Qt::CaseInsensitive)) {
      QString package = args.value("PKG");
      if (! package.isEmpty()) {
        PackageInfo pkgInfo = packageInfo(package);
        if (pkgInfo.isValid())
          _packageList << pkgInfo;
      }
    } else if (_currentCommand == CheckForUpdates
                 && ! type.compare("DEBUG")
                 && args.value("msg").startsWith("Got ")) {
      /* XXX: This is an even worse fucking kludge. Thandy only reports
       *      download progress in a not-so-parser-friendly log message,
       *      though, so we must kludge again.
       *
       *      Here's an example of what we're parsing:
       *        "Got 1666048/1666560 bytes from http://updates.torproject.org/thandy/data/win32/tor-0.2.1.9-alpha.msi"
       *
       *      (Note that the kludge above would even match on "Got milk?".)
       */
      QStringList parts = args.value("msg").split(" ");
      if (parts.size() == 5) {
        QStringList progress = parts.at(1).split("/");
        if (progress.size() == 2) {
          int bytesReceived = progress.at(0).toUInt();
          int bytesTotal = progress.at(1).toUInt();
          vInfo("updater: Downloaded %1 of %2 bytes of file %3").arg(bytesReceived)
                                                                .arg(bytesTotal)
                                                                .arg(parts.at(4));
          emit downloadProgress(parts.at(4), bytesReceived, bytesTotal);
        }
      }
    }
  }
}

void
UpdateProcess::readStandardOutput()
{
  QString line;

  setReadChannel(QProcess::StandardOutput);
  while (canReadLine()) {
    line = readLine().trimmed();
    vInfo("updater (stdout): %1").arg(line);
  }
}

void
UpdateProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  vInfo("updater: update process finished with exit code %1").arg(exitCode);

  if (_currentCommand == CheckForUpdates) {
    if (exitStatus == QProcess::NormalExit) {
      emit updatesAvailable(_currentBundle, _packageList);
    } else {
      emit checkForUpdatesFailed(tr("Vidalia was unable to check for available "
                                    "software updates because Tor's update process "
                                    "exited unexpectedly."));
    }
  } else if (_currentCommand == InstallUpdates) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0)
      emit updatesInstalled(_packageList.size());
  }
  _packageList.clear();
}

void
UpdateProcess::onError(QProcess::ProcessError error)
{
  if (error == QProcess::FailedToStart) {
    vWarn("updater: failed to start");
    emit checkForUpdatesFailed(tr("Vidalia was unable to check for available "
                                  "software updates because it could not find "
                                  "'%1'.").arg(updateExecutable()));
  }
}

int
UpdateProcess::checkForUpdatesInterval()
{
  /* XXX: Check twice a day. Why? Because arma said so. */
  return 12*60*60;
}

QDateTime
UpdateProcess::nextCheckForUpdates(const QDateTime &lastCheckedAt)
{
  return lastCheckedAt.addSecs(checkForUpdatesInterval()).toUTC();
}

bool
UpdateProcess::shouldCheckForUpdates(const QDateTime &lastCheckedAt)
{
  QDateTime nextCheckAt = nextCheckForUpdates(lastCheckedAt);
  return (QDateTime::currentDateTime().toUTC() >= nextCheckAt);
}

QString
UpdateProcess::updateExecutable()
{
  return "thandy.exe";
}

QString
UpdateProcess::updateRepositoryDir()
{
  return QDir::convertSeparators(Vidalia::dataDirectory() + "/updates");
}

QString
UpdateProcess::bundleInfoToString(BundleInfo bi)
{
  switch (bi) {
    case TorBundleInfo:
      return "/bundleinfo/tor/win32/";
    default:
      return QString();
  };
}

PackageInfo
UpdateProcess::packageInfo(const QString &package)
{
  QProcess proc;
  QStringList args;

  args << "json2xml"
       << QDir::convertSeparators(updateRepositoryDir() + "/" + package);

  vNotice("updater: launching auto-update executable: %1 %2")
                                           .arg(updateExecutable())
                                           .arg(args.join(" "));

  proc.setEnvironment(proc.systemEnvironment());
  proc.start(updateExecutable(), args);
  if (! proc.waitForStarted())
    return PackageInfo();
  if (! proc.waitForFinished())
    return PackageInfo();
  return packageInfoFromXml(proc.readAll());
}

PackageInfo
UpdateProcess::packageInfoFromXml(const QByteArray &xml)
{
  QDomDocument doc;
  QDomElement dict, elem;
  QDomNodeList nodeList;
  QString errmsg;
  QStringList versionParts;
  PackageInfo pkgInfo;

  if (! doc.setContent(xml, false, &errmsg, 0, 0))
    goto err;

  /* XXX: Qt 4.4 introduced XPath support, which would make the following
   * parsing much easier. Whenever we drop support for Qt < 4.4, this should
   * be updated.
   */
  elem = doc.documentElement().firstChildElement("signed");
  if (elem.isNull()) {
    errmsg = "Signed element not found";
    goto err;
  }

  dict = elem.firstChildElement("dict");
  if (dict.isNull()) {
    errmsg = "no Dict element as a child of Signed";
    goto err;
  }

  elem = dict.firstChildElement("name");
  if (elem.isNull()) {
    errmsg = "Name element not found";
    goto err;
  }
  pkgInfo.setName(elem.text());

  elem = dict.firstChildElement("version").firstChildElement("list");
  if (elem.isNull()) {
    errmsg = "no valid Version element found";
    goto err;
  }
  elem = elem.firstChildElement("item");
  for ( ; ! elem.isNull(); elem = elem.nextSiblingElement("item")) {
    versionParts << elem.text();
  }
  pkgInfo.setVersion(versionParts.join("."));

  elem = dict.firstChildElement("shortdesc").firstChildElement("dict");
  if (elem.isNull()) {
    errmsg = "no valid Shortdesc element found";
    goto err;
  }
  elem = elem.firstChildElement();
  for ( ; ! elem.isNull(); elem = elem.nextSiblingElement()) {
    pkgInfo.setShortDescription(elem.tagName(), elem.text());
  }

  elem = dict.firstChildElement("longdesc").firstChildElement("dict");
  if (elem.isNull()) {
    errmsg = "no valid Longdesc element found";
    goto err;
  }
  elem = elem.firstChildElement();
  for ( ; ! elem.isNull(); elem = elem.nextSiblingElement()) {
    pkgInfo.setLongDescription(elem.tagName(), elem.text());
  }

  return pkgInfo;

err:
  vWarn("updater: invalid package info XML document: %1").arg(errmsg);
  return PackageInfo();
}

