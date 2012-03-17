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
** \file GeoIpDatabase.h
** \brief Interface to a local MaxMind GeoIP database.
*/

#ifndef _GEOIPDATABASE_H
#define _GEOIPDATABASE_H

#include <GeoIP.h>
#include <GeoIPCity.h>

#include <QObject>

class QString;
class QHostAddress;
class GeoIpRecord;


class GeoIpDatabase : public QObject
{
  Q_OBJECT

public:
  enum DatabaseType {
    UnknownDatabase = 0,
    CountryDatabase,
    CityDatabase,
    RegionDatabase,
    OrganizationDatabase,
    IspDatabase,
    ProxyDatabase,
    AsnDatabase,
    NetSpeedDatabase,
    DomainDatabase
  };

  /** Default constructor.
   */
  GeoIpDatabase(QObject *parent = 0);

  /** Virtual destructor. Closes the database if it is currently open.
   */
  virtual ~GeoIpDatabase();

  /** Open the GeoIP database file <b>fname</b> and return true if
   * successful. Otherwise, return false. If a different database file is
   * already open, the open database will be closed before the new one is
   * opened.
   * \sa close()
   * \sa isOpen()
   */
  bool open(const QString &fname);

  /** Closes an open dataase, or does nothing if no database file is
   * currently open.
   * \sa open()
   * \sa isOpen()
   */
  void close();

  /** Return true if this object has a currently open GeoIP database.
   * \sa open()
   */
  bool isOpen() const;

  /** Returns the DatabaseType enum value corresponding to the current
   * database type. If no database is open, this will simply return
   * UnknownDatabase.
   */
  GeoIpDatabase::DatabaseType type() const;

  /** Resolves the IP address <b>ip</b> to its two-letter ISO-3166 country
   * code and returns the result on success. On failure, this returns a
   * default-constructed QString.
   */
  QString countryCodeByAddr(const QHostAddress &ip);

  /** Resolves the IP address <b>ip</b> to an approximate geographic
   * location and returns the result on success. On failure, this returns
   * a default-constructed QString.
   */
  GeoIpRecord recordByAddr(const QHostAddress &ip);

private:
  GeoIP *_db; /**< Pointer to the local GeoIP database object. */
};

#endif

