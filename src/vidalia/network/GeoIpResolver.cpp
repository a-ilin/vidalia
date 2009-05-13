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
** \file GeoIpResolver.cpp
** \version $Id$
** \brief Requests GeoIP information and caches the result
*/

#include "GeoIpResolver.h"
#include "GeoIpRequest.h"
#include "GeoIpResponse.h"
#include "GeoIp.h"
#include "Vidalia.h"

#include "stringutil.h"
#include "TorSslSocket.h"

/** Host for the GeoIP information. */ 
#define GEOIP_HOST    "geoips.vidalia-project.net"
/** The SSL GeoIP service runs on port 443. */
#define GEOIP_SSL_PORT  443
/** Page that we request the GeoIP information from. */
#define GEOIP_PAGE    "/cgi-bin/geoip.py"


/** Default constructor. */
GeoIpResolver::GeoIpResolver(QObject *parent)
  : QObject(parent)
{
  _socksAddr = QHostAddress::LocalHost;
  _socksPort = 9050;
  _cache = new GeoIpCache(this);
}

/** Sets the address and port of Tor, through which GeoIP requests will be
 * made. */
void
GeoIpResolver::setSocksHost(const QHostAddress &addr, quint16 port)
{
  _socksAddr = addr;
  _socksPort = port;
}

/** Resolves <b>ip</b> to geographic information if it is cached. A resolved()
 * signal will be emitted and true returned if we have cached geographic
 * information for <b>ip</b>. Otherwise, this returns false. */
bool
GeoIpResolver::resolveFromCache(const QHostAddress &ip)
{
  if (_cache->contains(ip)) {
    emit resolved(-1, QList<GeoIp>() << _cache->geoIpForAddress(ip));
    return true;
  }
  return false;
}

/** Resolves a list of IPs to a geographic location, but only those which are
 * cached. Returns a list of IPs that were not in the cache. */
QList<QHostAddress>
GeoIpResolver::resolveFromCache(const QList<QHostAddress> &ips)
{
  QList<GeoIp> cached;

  /* Build a list of which IPs have cached GeoIp information */
  foreach (QHostAddress ip, ips) {
    if (_cache->contains(ip))
      cached << _cache->geoIpForAddress(ip);
  }

  /* If any were cached, emit their results now */
  if (cached.size() > 0) {
    vInfo("Resolved %1 GeoIP entries from cache.").arg(cached.size());
    emit resolved(-1, cached);
  }
  return ips;
}

/** Resolves a single IP to a geographic location. */
int
GeoIpResolver::resolve(const QHostAddress &ip)
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
  GeoIpRequest *req = static_cast<GeoIpRequest *>(_requestList.value(socket));

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
  GeoIpRequest *req = static_cast<GeoIpRequest *>(_requestList.take(socket));

  /* Read and parse the response header */
  GeoIpResponse response = GeoIpResponse(socket->readAll());

  /* Check the response code and see what we got */
  if (response.statusCode() == 200) {
    /* We got a 200 OK, so get the Geo IP information from the response body
     * and cache the results. */
    parseGeoIpResponse(response.content(), req);
  } else {
    /* We failed to get the Geo IP information, so emit resolveFailed and
     * include the HTTP status message. */
    vWarn("GeoIP resolution failed (request id %1): %2").arg(req->id())
                                             .arg(response.statusMessage());
    emit resolveFailed(req->id(), response.statusMessage());
  }
  /* Close the socket and clean up */
  socket->close();
  delete socket;
  delete req;
}

void
GeoIpResolver::parseGeoIpResponse(const QByteArray &response,
                                  GeoIpRequest *request)
{
  QList<GeoIp> geoIpList;
  QHash<QString,QString> keyvals;
  QHostAddress ip, from, to;
  QString city, region, country, cc;
  float latitude, longitude;
  GeoIp geoIp;
  int numCached = 0;
  bool ok;

  QStringList lines = QString(response).split("\n", QString::SkipEmptyParts);
  foreach (QString line, lines) {
    /* Split the key=value formatted GeoIP record into keys and values */
    QHash<QString,QString> keyvals = string_parse_keyvals(line.trimmed(), &ok);
    if (! ok)
      goto err;

    /* Extract each of the required fields from the GeoIP record */
    ip = QHostAddress(keyvals.value("IP"));
    if (ip.isNull())
      goto err;
    latitude = keyvals.value("LAT").toFloat(&ok);
    if (! ok)
      goto err;
    longitude = keyvals.value("LON").toFloat(&ok);
    if (! ok)
      goto err;

    /* Each of these fields is optional */
    city    = keyvals.value("CITY");
    region  = keyvals.value("REGION");
    country = keyvals.value("COUNTRY");
    cc      = keyvals.value("CC");
    
    geoIp = GeoIp(ip, latitude, longitude, city, region, country, cc);
    if (! geoIp.isValid())
      goto err;

    if (request->contains(ip)) {
      if (! _cache->contains(ip)) {
        from = QHostAddress(keyvals.value("FROM"));
        to   = QHostAddress(keyvals.value("TO"));
        if (! from.isNull() && ! to.isNull())
          _cache->addToCache(from, to, geoIp);
        else
          _cache->addToCache(geoIp);
        numCached++;
      }

      geoIpList << geoIp;
      continue;

err:
      vInfo("Ignored improperly formatted GeoIP record (request id %1): %2")
                                               .arg(line).arg(request->id());
    } else {
      /* This item wasn't requested, so just log it and ignore. */
      vWarn("Received a GeoIP entry for IP address %1 that was not included "
            "in the initial request. (request id %2)").arg(ip)
                                                      .arg(request->id());
    }
  }
  /* If new results were cached, save them to disk */
  if (numCached > 0)
    _cache->saveToDisk();
  
  /* Emit the results */
  vInfo("Parsed %1 entries from the GeoIP response. (request id %2)")
                                   .arg(geoIpList.size()).arg(request->id());
  emit resolved(request->id(), geoIpList);  
}

/** Called when an error has occurred requesting Geo IP information. */
void
GeoIpResolver::socketError(const QString &errorString)
{
  /* Find the socket and request for whoever called this slot */ 
  QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>(sender());
  if (!_requestList.contains(socket)) {
    return;
  }
  
  /* We expect a remote host to close the socket, because that's how the HTTP
   * server tells us he's done talking to us. */
  if (socket->error() != QAbstractSocket::RemoteHostClosedError) {
    /* Emit the failure and clean up */
    GeoIpRequest *req = static_cast<GeoIpRequest *>(_requestList.take(socket));
    emit resolveFailed(req->id(), errorString);
    socket->abort();
    vWarn("GeoIP request socket error (request id %1): %2").arg(req->id())
                                                           .arg(errorString);
    delete socket;
    delete req;
  }
}

/** Creates an HTTP request for Geo IP information. */
GeoIpRequest*
GeoIpResolver::createRequest(const QList<QHostAddress> &ips)
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
GeoIpResolver::resolve(const QList<QHostAddress> &ips)
{
  /* Resolve the cached IPs and get a list of IPs that still need to be
   * resolved to a lat and long. */
  QList<QHostAddress> uncached = resolveFromCache(ips);
  if (! uncached.size())
    return -1;

  /* Create a socket used to request the geo ip information. */
  TorSslSocket *socket = new TorSslSocket(_socksAddr, _socksPort);
  connect(socket, SIGNAL(connectedToRemoteHost()), this, SLOT(connected()),
          Qt::QueuedConnection);
  connect(socket, SIGNAL(socketError(QString)), 
          this,   SLOT(socketError(QString)),
          Qt::QueuedConnection);
  connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()),
          Qt::QueuedConnection);
  GeoIpRequest *request = createRequest(uncached);
  _requestList.insert(socket, request);
  
  /* Connect so we can send our request and return the request ID. */
  vInfo("Opening an SSL connection to the GeoIP host at %1:%2 (request id %3)")
                        .arg(GEOIP_HOST).arg(GEOIP_SSL_PORT).arg(request->id());
  socket->connectToRemoteHost(GEOIP_HOST, GEOIP_SSL_PORT, true);

  return request->id();
}

