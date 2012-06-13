#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

#include <QDebug>

#include "VidaliaExtensions.h"

void vidalia_initialize_bindings(QScriptValue &);

QStringList VidaliaExtensions::keys() const
{
  return QStringList() << "vidalia";
}

void VidaliaExtensions::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("vidalia")) {
        QScriptValue extensionObject = engine->globalObject();
        vidalia_initialize_bindings(extensionObject);
    } else {
        Q_ASSERT_X(false, "vidalia::initialize", qPrintable(key));
    }
}

Q_EXPORT_PLUGIN2(vidalia_extensions, VidaliaExtensions)
