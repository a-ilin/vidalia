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
HelperProcessPrototype::metaTypeId() {
  return qMetaTypeId<HelperProcess *>();
}

QString
HelperProcessPrototype::name() {
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

