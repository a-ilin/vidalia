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
** \file geoipresolver.cpp
** \version $Id$
** \brief Requests GeoIP information and caches the result
*/

#include <torsocket.h>
#include <vidalia.h>
#include "geoipresolver.h"
#include "config.h"

#if defined(USE_QSSLSOCKET)
#include <torsslsocket.h>
#endif

/** Host for the geo ip information. */ 
#define GEOIP_HOST    "geoip.vidalia-project.net"
/** The non-encrypted GeoIP service lives on port 80. */
#define GEOIP_PORT      80
/** The SSL GeoIP service runs on port 1443 (443 was taken). */
#define GEOIP_SSL_PORT  1443
/** Page that we request the geo ip information from. */
#define GEOIP_PAGE    "/cgi-bin/geoip"


/** Sets the address and port of Tor, through which GeoIP requests will be
 * made. */
void
GeoIpResolver::setSocksHost(QHostAddress addr, quint16 port)
{
  _socksAddr = addr;
  _socksPort = port;
}

/** Resolves <b>ip</b> to geographic information if it is cached. A resolved()
 * signal will be emitted and true returned if we have cached geographic
 * information for <b>ip</b>. Otherwise, this returns false. */
bool
GeoIpResolver::resolveFromCache(QHostAddress ip)
{
  if (_cache.contains(ip)) {
    emit resolved(-1, QList<GeoIp>() << _cache.geoip(ip));
    return true;
  }
  return false;
}

/** Resolves a list of IPs to a geographic location, but only those which are
 * cached. Returns a list of IPs that were not in the cache. */
QList<QHostAddress>
GeoIpResolver::resolveFromCache(QList<QHostAddress> ips)
{
  QList<GeoIp> cached;

  /* Build a list of which IPs have cached GeoIp information */
  foreach (QHostAddress ip, ips) {
    if (_cache.contains(ip)) {
      ips.removeAt(ips.indexOf(ip));
      cached << _cache.geoip(ip);
    }
  }

  /* If any were cached, emit their results now */
  if (cached.size() > 0) {
    vInfo("Resolved %1 GeoIP entries from cache.").arg(ips.size());
    emit resolved(-1, cached);
  }
  return ips;
}

/** Resolves a single IP to a geographic location. */
int
GeoIpResolver::resolve(QHostAddress ip)
{
  return resolve(QList<QHostAddress>() << ip);
}

/** Called when the socket has connected to the Geo IP host. */
void
GeoIpResolver::connected()
{
  /* Find the socket and request for whoever called this slot */ 
  QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>(sender());
  if (!_requestList.contains(socket)) {
    return;
  }
  GeoIpRequest *req = (GeoIpRequest *)_requestList.value(socket);
  vInfo("Connected to the GeoIP host. Sending request for %1 uncached "
        "GeoIP entries. (request id %2)").arg(req->size()).arg(req->id());

  /* Send the request */
  socket->write(req->request());
}

/** Called when the socket has disconnected from the Geo IP host. */
void
GeoIpResolver::disconnected()
{
  /* Find the socket and request for whoever called this slot */ 
  QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>(sender());
  if (!_requestList.contains(socket)) {
    return;
  }
  GeoIpRequest *request = (GeoIpRequest *)_requestList.take(socket);

  /* Read and parse the response */
  GeoIpResponse response = GeoIpResponse(socket->readAll());

  /* Check the response code and see what we got */
  if (response.statusCode() == 200) {
    /* We got a 200 OK, so get the Geo IP information and cache the results */
    int numCached = 0, i = 0;
    QList<GeoIp> geoips = response.geoIps();
    foreach (GeoIp geoip, geoips) {
      QHostAddress ip = geoip.ip();
      
      if (request->contains(ip)) {
        /* This is a requested geoip item, so if it wasn't cached, then 
         * cache it now. */
        if (!_cache.contains(ip)) {
          _cache.cache(geoip);
          numCached++;
        }
        i++;
      } else {
        /* This item wasn't requested, so remove it. According to the Qt docs,
         * this is safe to do inside the foreach() loop because, "Qt
         * automatically takes a copy of the container when it enters a
         * foreach loop. If you modify the container as you are iterating,
         * that won't affect the loop." */
        vWarn("Received a GeoIP entry for IP address %1 that was not included "
              "in the initial request. (request id %2)").arg(ip)
                                                        .arg(request->id());
        geoips.removeAt(i);
      }
    }
    /* If new results were cached, save them to disk */
    if (numCached > 0) {
      _cache.saveToDisk();
    }
    /* Emit the results */
    vInfo("Parsed %1 entries from the GeoIP response. (request id %2)")
                                 .arg(geoips.size()).arg(request->id());
    emit resolved(request->id(), geoips);
  } else {
    /* We failed to get the Geo IP information, so emit resolveFailed and
     * include the HTTP status message. */
    vWarn("GeoIP resolution failed (request id %1): %2").arg(request->id())
                                             .arg(response.statusMessage());
    emit resolveFailed(request->id(), response.statusMessage());
  }
  /* Close the socket and clean up */
  socket->close();
  delete socket;
  delete request;
}

/** Called when an error has occurred requesting Geo IP information. */
void
GeoIpResolver::socketError(QString errorString)
{
  /* Find the socket and request for whoever called this slot */ 
  QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>(sender());
  if (!_requestList.contains(socket)) {
    return;
  }
  
  /* We expect a remote host to close the socket, because that's how the HTTP
   * server tells us he's done talkig to us. */
  if (socket->error() != QAbstractSocket::RemoteHostClosedError) {
    /* Emit the failure and clean up */
    GeoIpRequest *request = (GeoIpRequest *)_requestList.take(socket);
    emit resolveFailed(request->id(), errorString);
    socket->abort();
    vWarn("GeoIP request socket error (request id %1): %2").arg(request->id())
                                                           .arg(errorString);
    delete socket;
    delete request;
  }
}

/** Creates an HTTP request for Geo IP information. */
GeoIpRequest*
GeoIpResolver::createRequest(QList<QHostAddress> ips)
{
  static int id = -1;
  GeoIpRequest *request = new GeoIpRequest(++id);
  request->setHost(GEOIP_HOST);
  request->setPage(GEOIP_PAGE);
  request->setRequest(ips);
  return request;
}

/** Resolves a list of IPs to a geographic location. */
int
GeoIpResolver::resolve(QList<QHostAddress> ips)
{
  /* Resolve the cached IPs and get a list of IPs that still need to be
   * resolved to a lat and long. */
  QList<QHostAddress> uncached = resolveFromCache(ips);
  if (!uncached.size()) {
    return -1;
  }

  /* Create a socket used to request the geo ip information. */
#if defined(USE_QSSLSOCKET)
  TorSslSocket *socket = new TorSslSocket(_socksAddr, _socksPort);
#else
  TorSocket *socket = new TorSocket(_socksAddr, _socksPort);
#endif

  connect(socket, SIGNAL(connectedToRemoteHost()), this, SLOT(connected()),
          Qt::QueuedConnection);
  connect(socket, SIGNAL(socketError(QString)), 
          this,   SLOT(socketError(QString)),
          Qt::QueuedConnection);
  connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()),
          Qt::QueuedConnection);
  GeoIpRequest *request = createRequest(ips);
  _requestList.insert(socket, request);
  
  /* Connect so we can send our request and return the request ID. */
#if defined(USE_QSSLSOCKET)
  if (TorSslSocket::supportsSsl()) {
    vInfo("Opening an SSL connection to the GeoIP host at %1:%2 (request id %3)")
                          .arg(GEOIP_HOST).arg(GEOIP_SSL_PORT).arg(request->id());
    QByteArray caCert(":/geoip/cacert_root.crt");
    socket->addCaCertificate(QSslCertificate(caCert));
    socket->connectToRemoteHost(GEOIP_HOST, GEOIP_SSL_PORT, true);
  } else {
    vInfo("Opening an unencrypted connection to the GeoIP host at %1:%2 "
          "(request id %3)").arg(GEOIP_HOST).arg(GEOIP_PORT).arg(request->id());
    socket->connectToRemoteHost(GEOIP_HOST, GEOIP_PORT, false);
  }
#else
  vInfo("Opening an unencrypted connection to the GeoIP host at %1:%2 "
        "(request id %3)").arg(GEOIP_HOST).arg(GEOIP_PORT).arg(request->id());
  socket->connectToRemoteHost(GEOIP_HOST, GEOIP_PORT);
#endif
  return request->id();
}

