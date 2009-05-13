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
** \file GeoIpCacheItem.cpp
** \version $Id$
** \brief Cached result of a single IP-to-geolocation result
*/

#include "GeoIpCacheItem.h"
#include "GeoIp.h"

#include "stringutil.h"

#include <QString>
#include <QDateTime>
#include <QStringList>

#define CACHE_KEY_FROM_IP       "FROM"
#define CACHE_KEY_TO_IP         "TO"
#define CACHE_KEY_EXPIRES       "EXPIRES"
#define CACHE_KEY_LATITUDE      "LAT"
#define CACHE_KEY_LONGITUDE     "LON"
#define CACHE_KEY_CITY          "CITY"
#define CACHE_KEY_REGION        "REGION"
#define CACHE_KEY_COUNTRY       "COUNTRY"
#define CACHE_KEY_COUNTRY_CODE  "CC"


GeoIpCacheItem::GeoIpCacheItem()
{
  _fromIp = 0;
  _toIp = 0;
}

GeoIpCacheItem::GeoIpCacheItem(const QHostAddress &from, const QHostAddress &to,
                               const GeoIp &geoip, const QDateTime &expires)
{
  _fromIp = from.toIPv4Address();
  _toIp = to.toIPv4Address();
  _expires = expires;

  _fields.insert(CACHE_KEY_LATITUDE, geoip.latitude());
  _fields.insert(CACHE_KEY_LONGITUDE, geoip.longitude());
  if (! geoip.city().isEmpty())
    _fields.insert(CACHE_KEY_CITY, geoip.city());
  if (! geoip.region().isEmpty())
    _fields.insert(CACHE_KEY_REGION, geoip.region());
  if (! geoip.country().isEmpty())
    _fields.insert(CACHE_KEY_COUNTRY, geoip.country());
  if (! geoip.countryCode().isEmpty())
    _fields.insert(CACHE_KEY_COUNTRY_CODE, geoip.countryCode());
}

QHostAddress
GeoIpCacheItem::ipRangeStart() const
{
  return QHostAddress(_fromIp);
}

QHostAddress
GeoIpCacheItem::ipRangeEnd() const
{
  return QHostAddress(_toIp);
}

bool
GeoIpCacheItem::contains(const QHostAddress &ip) const
{
  quint32 ipv4 = ip.toIPv4Address();

  return (ipv4 >= _fromIp && ipv4 <= _toIp);
}

bool
GeoIpCacheItem::isValid() const
{
  return (_expires.isValid()
            && ! QHostAddress(_fromIp).isNull()
            && ! QHostAddress(_toIp).isNull()
            && _fromIp <= _toIp
            && _fields.contains(CACHE_KEY_LATITUDE)
            && _fields.contains(CACHE_KEY_LONGITUDE));
}

bool
GeoIpCacheItem::isExpired() const
{
  return (_expires < QDateTime::currentDateTime().toUTC());
}

GeoIp
GeoIpCacheItem::toGeoIp(const QHostAddress &ip) const
{
  if (this->contains(ip))
    return GeoIp(ip,
                 _fields.value(CACHE_KEY_LATITUDE).toDouble(),
                 _fields.value(CACHE_KEY_LONGITUDE).toDouble(),
                 _fields.value(CACHE_KEY_CITY).toString(),
                 _fields.value(CACHE_KEY_REGION).toString(),
                 _fields.value(CACHE_KEY_COUNTRY).toString(),
                 _fields.value(CACHE_KEY_COUNTRY_CODE).toString());
  return GeoIp();
}

QString
GeoIpCacheItem::toCacheString() const
{
  QStringList keyvals;

  keyvals << QString(CACHE_KEY_FROM_IP"=%1").arg(QHostAddress(_fromIp).toString());
  keyvals << QString(CACHE_KEY_TO_IP"=%1").arg(QHostAddress(_toIp).toString());
  keyvals << QString(CACHE_KEY_EXPIRES"=\"%1\"").arg(_expires.toString(Qt::ISODate));

  foreach (QString key, _fields.keys()) {
    QString value = _fields.value(key).toString();
    if (value.contains(" ")) {
      value.replace("\\", "\\\\");
      value.replace("\"", "\\\"");
      value = "\"" + value + "\"";
    }
    keyvals << key + "=" + value;
  }
  return keyvals.join(" ");
}

GeoIpCacheItem
GeoIpCacheItem::fromCacheString(const QString &line)
{
  GeoIpCacheItem ci;
  bool ok;

  QHash<QString,QString> keyvals = string_parse_keyvals(line, &ok);
  if (! ok)
    return GeoIpCacheItem();
  
  /* Get the range of IP addresses associated with this cache entry */
  QHostAddress fromIp(keyvals.value(CACHE_KEY_FROM_IP));
  QHostAddress toIp(keyvals.value(CACHE_KEY_TO_IP));
  if (fromIp.isNull() || toIp.isNull())
    return GeoIpCacheItem();
  ci._fromIp = fromIp.toIPv4Address();
  ci._toIp = toIp.toIPv4Address();

  /* Extract the expiration timestamp of this entry */
  ci._expires = QDateTime::fromString(keyvals.value(CACHE_KEY_EXPIRES),
                                   Qt::ISODate);
  if (! ci._expires.isValid())
    ci._expires = QDateTime::currentDateTime().toUTC().addDays(30);
  
  
  /* Make sure we have valid geographic coordinates */
  float latitude = keyvals.value(CACHE_KEY_LATITUDE).toFloat(&ok);
  if (! ok)
    return GeoIpCacheItem();
  ci._fields.insert(CACHE_KEY_LATITUDE, latitude);

  float longitude = keyvals.value(CACHE_KEY_LONGITUDE).toFloat(&ok);
  if (! ok)
    return GeoIpCacheItem();
  ci._fields.insert(CACHE_KEY_LONGITUDE, longitude);
  
  /* Each of these fields is optional */
  if (keyvals.contains(CACHE_KEY_CITY))
    ci._fields.insert(CACHE_KEY_CITY, keyvals.value(CACHE_KEY_CITY));
  if (keyvals.contains(CACHE_KEY_REGION))
    ci._fields.insert(CACHE_KEY_REGION, keyvals.value(CACHE_KEY_REGION));
  if (keyvals.contains(CACHE_KEY_COUNTRY))
    ci._fields.insert(CACHE_KEY_COUNTRY, keyvals.value(CACHE_KEY_COUNTRY));
  if (keyvals.contains(CACHE_KEY_COUNTRY_CODE))
    ci._fields.insert(CACHE_KEY_COUNTRY_CODE,
                      keyvals.value(CACHE_KEY_COUNTRY_CODE));

  return ci;
}

