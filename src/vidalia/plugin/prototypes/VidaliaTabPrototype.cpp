#include "VidaliaTabPrototype.h"

VidaliaTabPrototype::VidaliaTabPrototype(QObject *parent)
  : QObject(parent)
{}

QScriptValue
VidaliaTabPrototype::constructor(QScriptContext *context, QScriptEngine *engine)
{
  if((context->argumentCount() > 2) or (context->argumentCount() < 1))
    return QScriptValue(QScriptValue::NullValue);

  QString title = "";
  QString name = "";

  title = qscriptvalue_cast<QString>(context->argument(0));
  if(context->argumentCount() > 1)
    name = qscriptvalue_cast<QString>(context->argument(1));

  return engine->newQObject(new VidaliaTab(title, name), QScriptEngine::ScriptOwnership);
}

int 
VidaliaTabPrototype::metaTypeId() {
  return qMetaTypeId<VidaliaTab *>();
}

QString
VidaliaTabPrototype::name() {
  return QString("VidaliaTab");
}

void
VidaliaTabPrototype::setLayout(QLayout *layout)
{
  VidaliaTab *obj = qscriptvalue_cast<VidaliaTab *>(thisObject());

  if(obj)
    obj->setLayout(layout);
}

QVariant 
VidaliaTabPrototype::getSetting(QString name, QVariant defaultValue)
{
  VidaliaTab *obj = qscriptvalue_cast<VidaliaTab *>(thisObject());

  if(obj)
    return obj->getSetting(name, defaultValue);
}

void 
VidaliaTabPrototype::saveSetting(QString name, QVariant value)
{
  VidaliaTab *obj = qscriptvalue_cast<VidaliaTab *>(thisObject());

  if(obj)
    obj->saveSetting(name, value);
}
