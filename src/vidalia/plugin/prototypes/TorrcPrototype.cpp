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
** \file TorrcPrototype.cpp
** \brief Prototype for Torrc class
*/

#include "TorrcPrototype.h"
#include "prototypemacros.h"

TorrcPrototype::TorrcPrototype()
  : QObject(), QScriptable() {}

int
TorrcPrototype::metaTypeId()
{
  return qMetaTypeId<Torrc *>();
}

DEF_TYPE0(Torrc, void,
    load(const QString &torrcPath, const QString &defaultsPath = ""),
    load(torrcPath, defaultsPath))

bool
TorrcPrototype::apply(TorControl *tc, QString *errmsg){
  Torrc *obj = qscriptvalue_cast<Torrc *>(thisObject());

  if(obj)
    return obj->apply(tc, errmsg);

  return false;
}

DEF_TYPE0(Torrc, void,
    clear(QStringList keys),
    clear(keys))

DEF_TYPE0(Torrc, void,
    clearAll(),
    clearAll())

DEF_TYPE0(Torrc, QStringList,
    value(const QString &key) const,
    value(key))

DEF_TYPE0(Torrc, void,
    setValue(const QString &key, const QString &value, const QString &comment),
    setValue(key, value, comment))

DEF_TYPE0(Torrc, QString,
    getTorrcPath() const,
    getTorrcPath())

DEF_TYPE0(Torrc, bool,
    isValid(const QString &key, const QString &value),
    isValid(key, value))


bool
TorrcPrototype::setRawContents(const QString &contents, QString *errmsg, TorControl *tc){
  Torrc *obj = qscriptvalue_cast<Torrc *>(thisObject());

  if(obj)
    return obj->setRawContents(contents, errmsg, tc);

  return false;
}
