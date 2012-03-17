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
G** \file GeoIpResolver.h
** \brief Retrieves GeoIP information either from Tor or from a local
** database and returns the result.
*/

#ifndef _GEOIPRESOLVER_H
#define _GEOIPRESOLVER_H

#include "Vidalia.h"
#ifdef USE_GEOIP
#include "GeoIpDatabase.h"
#endif
#include "CountryInfo.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QHostAddress>

class QString;
class GeoIpRecord;


class GeoIpResolver : public QObject
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  GeoIpResolver(QObject *parent = 0);

  /** Sets the local database file to <b>databaseFile</b>. Returns true if
   * <b>databaseFile</b> could be opened for reading. Otherwise, returns
   * false.
   * \sa setUseLocalDatabase()
   */
  bool setLocalDatabase(const QString &databaseFile);

  /** Enables or disables the use of a local GeoIP database, depending on
   * the value of <b>useLocalDatabase</b>.
   * \sa setLocalDatabase()
   */
  void setUseLocalDatabase(bool useLocalDatabase);

  /** Resolves a single IP to a geographic location and returns the
   * result on success. On failure, this returns a default-constructed
   * GeoIpRecord object.
   */
  GeoIpRecord resolve(const QHostAddress &ip);

protected:
  /** Maps <b>ip</b> to a country code using Tor, and then maps the
   * country code to a geographic location using the built-in
   * country-to-coordinate database.
   */
  GeoIpRecord resolveUsingTor(const QHostAddress &ip);

  /** Maps <b>ip</b> to an approximate geographic location using a local
   * GeoIP database and returns the result on success.
   * \sa setLocalDatabase()
   * \sa setUseLocalDatabase()
   */
  GeoIpRecord resolveUsingLocalDatabase(const QHostAddress &ip);

private:
#ifdef USE_GEOIP
  /** Wrapper around a local database used for looking up GeoIP
   * information.
   */
  GeoIpDatabase _database;
#endif
  bool _useLocalDatabase;
};

#endif

