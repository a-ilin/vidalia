#ifndef VIDALIAEXTENSIONS_H
#define VIDALIAEXTENSIONS_H

#include <QtCore>
#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

class VidaliaExtensions : public QScriptExtensionPlugin
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QScriptExtensionPlugin")
#endif

public:
  void initialize(const QString &key, QScriptEngine *engine);
  QStringList keys() const;
};


#endif // VIDALIAEXTENSIONS_H
