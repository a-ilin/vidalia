#ifndef PLUGINENGINE_H
#define PLUGINENGINE_H

#include <QtGui>
#include <QtScript>

#include "VidaliaTabPrototype.h"
#include "HelperProcessPrototype.h"
//#include "TorControlPrototype.h"

class PluginWrapper;

#define ADD_PROTOTYPE(protoType) \
  QScriptValue __script##protoType = newQObject(new protoType); \
  setDefaultPrototype(protoType::metaTypeId(), __script##protoType); \

#define MAKE_CREATABLE(protoType) \
  globalObject().setProperty(protoType::name(), newFunction(protoType::constructor, __script##protoType));

class PluginEngine : public QScriptEngine {
  Q_OBJECT

  public:
    PluginEngine(QObject *parent = 0);
    ~PluginEngine();

    QList<QAction *> getAllActions();

  signals:
    void pluginTab(VidaliaTab *);

  protected:
    static QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine);
    static bool loadFile(QString fileName, QScriptEngine *engine);
    static QScriptValue includeScript(QScriptContext *context, QScriptEngine *engine);

    void loadAllPlugins();
    void tryLoadPlugin(QDir path);

    QList<PluginWrapper *> wrappers;
};

#endif

