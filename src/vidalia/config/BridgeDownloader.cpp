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
** \file BridgeDownloader.cpp
** \brief Downloads a list of new bridge addresses via HTTPS
*/

#include "BridgeDownloader.h"
#include "Vidalia.h"

#define BRIDGEDB_HOST  "bridges.torproject.org"
#define BRIDGEDB_PORT  443

BridgeDownloader::BridgeDownloader(QObject *parent)
  : QObject(parent)
{
  _https = new QNetworkAccessManager();

  connect(_https, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(httpsRequestFinished(QNetworkReply *)));
  connect(_https, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)),
          this, SLOT(sslErrors(QNetworkReply *, QList<QSslError>)));
}

void
BridgeDownloader::setProxy(const QString &host, int port,
                           const QString &username, const QString &password)
{
  _https->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, host, port, username, password));
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

  _reply = _https->get(QNetworkRequest(QUrl("https://bridges.torproject.org/?format=plain")));
  connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
          this, SIGNAL(downloadProgress(qint64, qint64)));
  vInfo("Sending an HTTPS bridge request to %1:%2.").arg(BRIDGEDB_HOST)
                                                    .arg(BRIDGEDB_PORT);
}

void
BridgeDownloader::cancelBridgeRequest()
{
  _reply->close();
  disconnect(_reply, 0, 0, 0);
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
BridgeDownloader::httpsRequestFinished(QNetworkReply *reply)
{
  if (reply->error() != QNetworkReply::NoError) {
    QString errorString = reply->errorString();
    vWarn("Bridge request failed: %2").arg(errorString);

    emit bridgeRequestFailed(errorString);
  } else {
    QByteArray response = reply->readAll();
    vInfo("Bridge request complete: received %2 bytes.").arg(response.size());

    QStringList bridges, lines = QString(response).split("\n");
    foreach (QString line, lines) {
      line = line.trimmed();
      if (line.startsWith("bridge ", Qt::CaseInsensitive))
        bridges << line;
    }
    emit bridgeRequestFinished(bridges);
  }
  _reply->close();
  disconnect(_reply,0,0,0);
}

void
BridgeDownloader::sslErrors(QNetworkReply *reply, const QList<QSslError> &sslErrors)
{
  QString errorString;
  QStringList errorStrings;

  vWarn("%1 SSL error(s) when requesting bridge information:")
                                                      .arg(sslErrors.size());
  foreach (QSslError sslError, sslErrors) {
    errorString = sslError.errorString();
    errorStrings << errorString;
    vWarn("  SSL Error: %1").arg(errorString);
  }
}
