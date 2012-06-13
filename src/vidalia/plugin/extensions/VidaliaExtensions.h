#ifndef VIDALIAEXTENSIONS_H
#define VIDALIAEXTENSIONS_H

#include <QtCore>
#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

class VidaliaExtensions : public QScriptExtensionPlugin
{
public:
  void initialize(const QString &key, QScriptEngine *engine);
  QStringList keys() const;
};


#endif // VIDALIAEXTENSIONS_H
