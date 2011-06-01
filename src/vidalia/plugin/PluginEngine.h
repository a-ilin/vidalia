#ifndef PLUGINENGINE_H
#define PLUGINENGINE_H

#include <QtGui>
#include <QtScript>

#include "VidaliaTabPrototype.h"

class PluginWrapper;

class PluginEngine : public QScriptEngine {
  Q_OBJECT

  public:
    PluginEngine(QObject *parent = 0);
    ~PluginEngine();

    QList<QAction *> getAllActions();

  protected:
    void loadAllPlugins();
    void tryLoadPlugin(QDir path);

    QList<PluginWrapper *> wrappers;
};

#endif

