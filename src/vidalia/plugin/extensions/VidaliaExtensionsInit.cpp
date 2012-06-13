#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

const int classCount = 1;

QScriptValue qtscript_create_TorControl_class(QScriptEngine *engine);

static const char * const vidalia_class_names[] = {
  "TorControl"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator vidalia_class_functions[] = {
  qtscript_create_TorControl_class
};

void vidalia_initialize_bindings(QScriptValue &extensionObject)
{
    QScriptEngine *engine = extensionObject.engine();
    for (int i = 0; i < classCount; ++i) {
        extensionObject.setProperty(vidalia_class_names[i],
            vidalia_class_functions[i](engine),
            QScriptValue::SkipInEnumeration);
    }
}
