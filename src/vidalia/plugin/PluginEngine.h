#ifndef PLUGINENGINE_H
#define PLUGINENGINE_H

#include <QtGui>
#include <QtScript>

#include "VidaliaTabPrototype.h"

class PluginWrapper;

#define ADD_CLASS(name, protoType, absType, ctor) \
  protoType __proto; \
  QScriptValue __script = newQObject(&__proto, QScriptEngine::ScriptOwnership); \
  setDefaultPrototype(qMetaTypeId<absType>(), __script); \
  QScriptValue __ctor = newFunction(ctor, __script); \
  globalObject().setProperty(name, __ctor);

class PluginEngine : public QScriptEngine {
  Q_OBJECT

  public:
    PluginEngine(QObject *parent = 0);
    ~PluginEngine();

    QList<QAction *> getAllActions();

  signals:
    void pluginTab(VidaliaTab *);

  protected:
    void loadAllPlugins();
    void tryLoadPlugin(QDir path);

    QList<PluginWrapper *> wrappers;
};

#endif

