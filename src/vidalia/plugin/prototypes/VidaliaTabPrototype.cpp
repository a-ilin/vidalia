#include "VidaliaTabPrototype.h"

VidaliaTabPrototype::VidaliaTabPrototype(QObject *parent)
  : QObject(parent)
{}

QScriptValue VidaliaTabPrototype::constructor(QScriptContext *context, QScriptEngine *engine)
{
  return engine->newQObject(new VidaliaTab(QString("titulooo"), QString("nombreee")), QScriptEngine::ScriptOwnership);
}
