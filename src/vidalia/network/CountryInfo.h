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
** \file CountryInfo.h
** \brief Provides a method to look up a localized country name given its
** ISO 3166-1 2-letter country code.
*/

#ifndef _COUNTRYINFO_H
#define _COUNTRYINFO_H

#include <QObject>
#include <QString>
#include <QPair>


class CountryInfo : public QObject
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  CountryInfo(QObject *parent = 0);

  /** Returns the name of the country represented by <b>countryCode</b>, where
   * <b>countryCode</b> is a 2-letter ISO 3166-1 alpha-2 two-letter country
   * code. The name will be returned translated to the current locale if an
   * appropriate QTranslator is currently installed.
   */
  static QString countryName(const QString &countryCode);

  /**
   */
  static QPair<float,float> countryLocation(const QString &countryCode);
};

#endif

