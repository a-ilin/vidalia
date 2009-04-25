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
** \file bridgedownloader.cpp
** \version $Id$
** \brief Downloads a list of new bridge addresses via HTTPS
*/

#include <QSslSocket>
#include <vidalia.h>
#include <bridgedownloader.h>

#define BRIDGEDB_HOST  "bridges.torproject.org"
#define BRIDGEDB_PORT  443


BridgeDownloader::BridgeDownloader(QObject *parent)
  : QObject(parent),
    _requestId(0)
{
  _https = new QHttp(BRIDGEDB_HOST,  QHttp::ConnectionModeHttps,
                     BRIDGEDB_PORT, this);

  connect(_https, SIGNAL(stateChanged(int)),
          this, SLOT(httpsStateChanged(int)));
  connect(_https, SIGNAL(requestFinished(int, bool)),
          this, SLOT(httpsRequestFinished(int, bool)));
  connect(_https, SIGNAL(dataReadProgress(int, int)),
          this, SIGNAL(downloadProgress(int, int)));
  connect(_https, SIGNAL(sslErrors(QList<QSslError>)),
          this, SLOT(sslErrors(QList<QSslError>)));  
}

void
BridgeDownloader::setProxy(const QString &host, int port,
                           const QString &username, const QString &password)
{
  _https->setProxy(host, port, username, password);
}

bool
BridgeDownloader::downloadBridges(BridgeDownloadMethod method)
{
  if (! isMethodSupported(method))
    return false;

  switch (method) {
    case DownloadMethodHttps:
      startHttpsDownload();
      break;
 
    default:
      break;
  }
  return true;
}

bool
BridgeDownloader::isMethodSupported(BridgeDownloadMethod method)
{
  switch (method) {
    case DownloadMethodHttps:
      return QSslSocket::supportsSsl();

    default:
      break;
  }
  return false;
}

void
BridgeDownloader::startHttpsDownload()
{  
  emit statusChanged(tr("Starting HTTPS bridge request..."));
  emit downloadProgress(0, 0);

  _requestId = _https->get("/?format=plain");
  vInfo("Sending an HTTPS bridge request to %1:%2 (id %3).").arg(BRIDGEDB_HOST)
                                                            .arg(BRIDGEDB_PORT)
                                                            .arg(_requestId);
}

void
BridgeDownloader::cancelBridgeRequest()
{
  _https->abort();
}

void
BridgeDownloader::httpsStateChanged(int state)
{
  switch (state) {
    case QHttp::Connecting:
      emit statusChanged(tr("Connecting to %1:%2...").arg(BRIDGEDB_HOST)
                                                     .arg(BRIDGEDB_PORT));
      break;

    case QHttp::Sending:
      emit statusChanged(tr("Sending an HTTPS request for bridges..."));
      break;

    case QHttp::Reading:
      emit statusChanged(tr("Downloading a list of bridges..."));
      break;

    default:
      break;
  }
}

void
BridgeDownloader::httpsRequestFinished(int id, bool error)
{
  if (id != _requestId)
    return;

  if (error) {
    QString errorString = _https->errorString();
    vWarn("Bridge request failed (id %1): %2").arg(id).arg(errorString);
  
    emit bridgeRequestFailed(errorString);
  } else {
    QByteArray response = _https->readAll();
    vInfo("Bridge request complete (id %1): received %2 bytes.").arg(id)
                                                   .arg(response.size());

    QStringList bridges, lines = QString(response).split("\n");
    foreach (QString line, lines) {
      line = line.trimmed();
      if (line.startsWith("bridge ", Qt::CaseInsensitive))
        bridges << line;
    }
    emit bridgeRequestFinished(bridges);
  }
  _https->close();
}

void
BridgeDownloader::sslErrors(const QList<QSslError> &sslErrors)
{
  QString errorString;
  QStringList errorStrings;

  vWarn("%1 SSL error(s) when requesting bridge information (id %2):")
                                                      .arg(sslErrors.size())
                                                      .arg(_requestId);
  foreach (QSslError sslError, sslErrors) {
    errorString = sslError.errorString();
    errorStrings << errorString;
    vWarn("  SSL Error: %1").arg(errorString);
  }
}

