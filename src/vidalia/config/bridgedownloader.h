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
** \file bridgedownloader.h
** \version $Id$
** \brief Downloads a list of new bridge addresses via HTTPS
*/

#ifndef _BRIDGEDOWNLOADER_H
#define _BRIDGEDOWNLOADER_H

#include <QHttp>
#include <QSslError>
#include <QStringList>


class BridgeDownloader : public QObject
{
  Q_OBJECT

public:
  /** Available bridge download methods. */
  enum BridgeDownloadMethod {
    DownloadMethodHttps, /** Download via an HTTPS connection. */
  };

  /** Default constructor.
   */
  BridgeDownloader(QObject *parent = 0);

  /** Initiates a request for a set of bridges using the specified
   * download <b>method</b>. Returns true if the request was initiated
   * successfully, or false on error.
   */
  bool downloadBridges(BridgeDownloadMethod method);

  /** Returns true if <b>method</b> is supported by the currently
   * available Qt libraries.
   */
  static bool isMethodSupported(BridgeDownloadMethod method);

public slots:
  /** Cancels any pending bridge download requests.
   */
  void cancelBridgeRequest();

signals:
  /** Emitted when the underlying QHttp object reads data from an HTTPS
   * response. <b>done</b> indicates how many bytes out of <b>total</b>
   * have been read so far. Note that <b>total</b> may be 0 if the expected
   * total size of the response is not known.
   */
  void downloadProgress(int done, int total);

  /** Emitted when the status of the bridge request changes. <b>status</b>
   * describes the new current state of the request.
   */
  void statusChanged(const QString &status);

  /** Emitted when the previous request for bridge addresses completes
   * successfully. The QStringList <b>bridges</b> contains a (possibly empty)
   * list of bridge addresses parsed from the received response.
   */
  void bridgeRequestFinished(const QStringList &bridges);

  /** Emitted when the previous request for bridge addresses fails. The
   * QString <b>error</b> is a human-readable string describing the error
   * encountered.
   */
  void bridgeRequestFailed(const QString &error);

private slots:
  /** Called when the state of the underlying QHttp object changes. A
   * statusChanged() signal is emitted with the appropriate text
   * describing the new state of the request.
   */
  void httpsStateChanged(int state); 

  /** Called when the underlying QHttp object used to make the bridge
   * request completes. <b>error</b> is set to false if the request was
   * successful, or true if the request failed. If <b>id</b> does not
   * match the request ID previously returned by QHttp::get(), then the
   * signal is ignored since it is the result of a close() or abort()
   * request.
   */
  void httpsRequestFinished(int id, bool error);

  /** Called when the HTTPS connection encounters one or more
   * <b>sslErrors</b>. Currently the errors are just logged and
   * bridgeRequestFailed() is <i>not</i> emitted, since QHttp will also
   * emit 
   */
  void sslErrors(const QList<QSslError> &sslErrors);

private:
  /** Initiates an HTTPS connection to bridges.torproject.org to start
   * downloading a set of bridges.
   */
  void startHttpsDownload();

  /** Used to connect to the bridge database, send an HTTPS request for
   * new bridge addresses and then read the response. */
  QHttp* _https;
  
  /** Unique numeric identifier of the current bridge request. */
  int _requestId;
};

#endif

