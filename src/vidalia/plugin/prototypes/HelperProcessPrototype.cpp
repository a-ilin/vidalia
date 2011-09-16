/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/* 
** \file HelperProcessPrototype.cpp
** \brief Prototype for the HelperProcess class
*/

#include "HelperProcessPrototype.h"

HelperProcessPrototype::HelperProcessPrototype(QObject *parent)
  : QObject(parent)
{}

QScriptValue
HelperProcessPrototype::constructor(QScriptContext *context, QScriptEngine *engine)
{
  return engine->newQObject(new HelperProcess(), QScriptEngine::ScriptOwnership);
}

int 
HelperProcessPrototype::metaTypeId()
{
  return qMetaTypeId<HelperProcess *>();
}

QString
HelperProcessPrototype::name()
{
  return QString("HelperProcess");
}

void 
HelperProcessPrototype::start(const QString &app, const QString &args)
{
  HelperProcess *obj = qscriptvalue_cast<HelperProcess *>(thisObject());

  if(obj)
    obj->start(app, args);
}

void 
HelperProcessPrototype::start(const QString &app, const QStringList &args)
{
  HelperProcess *obj = qscriptvalue_cast<HelperProcess *>(thisObject());

  if(obj)
    obj->start(app, args);
}

bool 
HelperProcessPrototype::isDone() const
{
  HelperProcess *obj = qscriptvalue_cast<HelperProcess *>(thisObject());

  if(obj)
    return obj->isDone();
}

QStringList
HelperProcessPrototype::systemEnvironment()
{
  return HelperProcess::systemEnvironment();
}

void
HelperProcessPrototype::setEnvironment(const QStringList &env)
{
  HelperProcess *obj = qscriptvalue_cast<HelperProcess *>(thisObject());

  if(obj)
    obj->setEnvironment(env);
}

HelperProcess::ProcessState
HelperProcessPrototype::state()
{
  HelperProcess *obj = qscriptvalue_cast<HelperProcess *>(thisObject());

  if(obj)
    return obj->state();

  return HelperProcess::NotRunning;
}

void
HelperProcessPrototype::toForeground()
{
  HelperProcess *obj = qscriptvalue_cast<HelperProcess *>(thisObject());

  if(obj)
    obj->toForeground();
}
