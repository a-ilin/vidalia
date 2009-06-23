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
** \file CrashReportUploader.cpp
** \version $Id$
** \brief Uploads a minidump file and any extra information to a crash
** reporting server.
*/

#include "CrashReportUploader.h"

#include <QtGlobal>
#include <QDateTime>
#include <QSslSocket>
#include <QSslCertificate>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QMap>
#include <QUrl>


CrashReportUploader::CrashReportUploader(QObject *parent)
  : QObject(parent),
    _requestId(-1)
{
  /* Clear the default CA certificate store and add the only one we want */
  QSslSocket::setDefaultCaCertificates(QList<QSslCertificate>());
  QSslSocket::addDefaultCaCertificates(":/pki/gd-class2-root.crt");

  /* Create the QHttp object used to talk to the crash reporting server */
  _http = new QHttp(this);
  connect(_http, SIGNAL(stateChanged(int)),
          this, SLOT(httpStateChanged(int)));
  connect(_http, SIGNAL(requestFinished(int, bool)),
          this, SLOT(httpRequestFinished(int, bool)));
  connect(_http, SIGNAL(dataSendProgress(int, int)),
          this, SIGNAL(uploadProgress(int, int)));

  /* Seed the lame PRNG we'll use to generate the multipart boundary marker */
  qsrand(QDateTime::currentDateTime().toTime_t());
}

void
CrashReportUploader::uploadMinidump(const QUrl &serverUrl,
                                    const QString &minidumpId,
                                    const QByteArray &minidump,
                                    const QMap<QString,QString> &parameters)
{
  QByteArray body;

  /* Set the destination host. If it looks like the destination URL uses SSL,
   * then we need to tell the QHttp object to use it as well. */
  if (! serverUrl.scheme().compare("https", Qt::CaseInsensitive)) {
    _http->setHost(serverUrl.host(), QHttp::ConnectionModeHttps,
                   serverUrl.port(443));
  } else {
    _http->setHost(serverUrl.host(), QHttp::ConnectionModeHttp,
                   serverUrl.port(80));
  }

  /* Set up the request header */
  QHttpRequestHeader header("POST", serverUrl.path());
  QString boundary = generateBoundaryMarker();
  header.setValue("Host", serverUrl.host());
  header.setContentType(QString("multipart/form-data; boundary=%1")
                                                    .arg(boundary));

  /* Add all the key=value parameters to the request body */
  foreach (QString key, parameters.keys()) {
    QString value = parameters.value(key);
    if (! value.isEmpty()) {
      body.append(QString("--%1\r\n").arg(boundary));
      body.append(QString("Content-Disposition: form-data; name=\"%1\"").arg(key));
      body.append("\r\n\r\n");
      body.append(value.toUtf8());
      body.append("\r\n");
    }
  }

  /* Append the minidump contents */
  body.append(QString("--%1\r\n").arg(boundary));
  body.append("Content-Disposition: form-data; ");
  body.append("name=\"upload_file_minidump\"; ");
  body.append(QString("filename=\"%1\"\r\n").arg(minidumpId));
  body.append("Content-Type: application/octet-stream\r\n\r\n");
  body.append(minidump);
  body.append(QString("\r\n--%1\r\n").arg(boundary));

  /* Initiate the request and return the request ID */
  _requestId = _http->request(header, body);
}

QString
CrashReportUploader::generateBoundaryMarker() const
{
  return QString("%1%2").arg((quint32)qrand(), 8, 16, QChar('0'))
                        .arg((quint32)qrand(), 8, 16, QChar('0'));  
}

void
CrashReportUploader::cancel()
{
  _http->abort();
}

void
CrashReportUploader::httpStateChanged(int state)
{
  switch (state) {
    case QHttp::Connecting:
      emit statusChanged(tr("Connecting..."));
      break;

    case QHttp::Sending:
      emit statusChanged(tr("Sending crash report..."));
      break;

    case QHttp::Reading:
      emit statusChanged(tr("Receiving response..."));
      break;

    default:
      break;
  }
}

void
CrashReportUploader::httpRequestFinished(int id, bool error)
{
  if (id != _requestId)
    return;

  if (error) {
    QString errorString = _http->errorString();
    emit uploadFailed(errorString);
  } else {
    QHttpResponseHeader response = _http->lastResponse();
    if (response.statusCode() == 200)
      emit uploadFinished();
    else
      emit uploadFailed(response.reasonPhrase());
  }
  _http->close();
}

