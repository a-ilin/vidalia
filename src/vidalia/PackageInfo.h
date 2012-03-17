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
** \file PackageInfo.h
** \brief Contains information about a single available updated software
** package.
*/

#ifndef _PACKAGEINFO_H
#define _PACKAGEINFO_H

#include <QHash>
#include <QList>
#include <QString>


class PackageInfo
{
public:
  /** Default constructor. */
  PackageInfo();

  /** Returns true if this PackageInfo object is valid. A valid PackageInfo
   * object must have a name and a version number set. All other fields are
   * optional.
   */
  bool isValid() const;

  /** Sets the name of this software package to <b>name</b>.
   */
  void setName(const QString &name);

  /** Returns the name of this software package.
   */
  QString name() const;

  /** Sets the version of this software package to <b>version</b>.
   */
  void setVersion(const QString &version);

  /** Returns the version of this software package.
   */
  QString version() const;

  /** Sets the long description of this software package to <b>desc</b> for
   * the language <b>lang</b>.
   */
  void setLongDescription(const QString &lang, const QString &desc);

  /** Returns true if there is a long description for this software package
   * currently set for language <b>lang</b>.
   */
  bool hasLongDescription(const QString &lang) const;

  /** Returns long description of this software package for language
   * <b>lang</b>. If a description is not currently set for the specified
   * language, a null QString object is returned.
   */
  QString longDescription(const QString &lang) const;

  /** Sets the short description of this software package to <b>desc</b> for
   * the language <b>lang</b>.
   */
  void setShortDescription(const QString &lang, const QString &desc);

  /** Returns true if there is a short description of this software package
   * currently set for language <b>lang</b>.
   */
  bool hasShortDescription(const QString &lang) const;

  /** Returns the short description of this software package for language
   * <b>lang</b>. If a description is not currently set for the specified
   * language, a null QString object is returned.
   */
  QString shortDescription(const QString &lang) const;

private:
  QString _name;
  QString _version;
  QHash<QString,QString> _longDescription;
  QHash<QString,QString> _shortDescription;
};

/** An unordered collection of PackageInfo objects. */
typedef QList<PackageInfo> PackageList;

#endif

