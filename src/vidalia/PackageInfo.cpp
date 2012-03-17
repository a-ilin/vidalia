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
** \file PackageInfo.cpp
** \brief Contains information about a single available updated software
** package.
*/

#include "Packageinfo.h"


PackageInfo::PackageInfo()
{
}

bool
PackageInfo::isValid() const
{
  return (! _name.isEmpty() && ! _version.isEmpty());
}

void
PackageInfo::setName(const QString &name)
{
  _name = name;
}

QString
PackageInfo::name() const
{
  return _name;
}

void
PackageInfo::setVersion(const QString &version)
{
  _version = version;
}

QString
PackageInfo::version() const
{
  return _version;
}

void
PackageInfo::setLongDescription(const QString &lang, const QString &desc)
{
  _longDescription.insert(lang, desc);
}

QString
PackageInfo::longDescription(const QString &lang) const
{
  return _longDescription.value(lang);
}

bool
PackageInfo::hasLongDescription(const QString &lang) const
{
  return _longDescription.contains(lang);
}

void
PackageInfo::setShortDescription(const QString &lang, const QString &desc)
{
  _shortDescription.insert(lang, desc);
}


QString
PackageInfo::shortDescription(const QString &lang) const
{
  return _shortDescription.value(lang);
}


bool
PackageInfo::hasShortDescription(const QString &lang) const
{
  return _shortDescription.contains(lang);
}

