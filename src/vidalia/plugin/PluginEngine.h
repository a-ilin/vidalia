/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file PluginEngine.h
** \brief Engine that handles all plugin related features
*/

#ifndef PLUGINENGINE_H
#define PLUGINENGINE_H

#include <QtGui>
#include <QtScript>

#include "HelperProcessPrototype.h"
#include "TorrcPrototype.h"
#include "VidaliaTab.h"

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

    void loadAllPlugins();
    QList<QAction *> getAllActions();
    void provide(const QString &name, QObject *obj);

  signals:
    void pluginTab(VidaliaTab *);

  protected:
    static QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine);
//    static bool loadFile(QString fileName, QScriptEngine *engine);
//    static QScriptValue includeScript(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue vdebug(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue findWidget(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue sleep(QScriptContext *context, QScriptEngine *engine);

    void tryLoadPlugin(QDir path);

    QList<PluginWrapper *> wrappers;
};

#endif

