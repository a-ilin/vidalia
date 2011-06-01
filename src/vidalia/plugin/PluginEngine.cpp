#include "PluginEngine.h"
#include "VidaliaSettings.h"
#include "PluginWrapper.h"

PluginEngine::PluginEngine(QObject *parent)
  : QScriptEngine(parent)
{
  // load prototypes
  VidaliaTabPrototype vtabproto;
  QScriptValue vtabscript = newQObject(&vtabproto, QScriptEngine::ScriptOwnership);
  setDefaultPrototype(qMetaTypeId<VidaliaTab *>(), vtabscript);

  // load constructors
  QScriptValue vtabctor = newFunction(VidaliaTabPrototype::constructor, vtabscript);
  globalObject().setProperty("VidaliaTab", vtabctor);

  loadAllPlugins();
}

PluginEngine::~PluginEngine() {}

void
PluginEngine::loadAllPlugins()
{
  qWarning() << "loadAllPlugins()";

  VidaliaSettings settings;
  QDir path = QDir(settings.pluginPath());

  qWarning() << "PluginPath" << path.absolutePath();
  
  foreach(QString pdir, path.entryList(QDir::NoDotAndDotDot|QDir::AllDirs)) {
    qWarning() << "pdir" << pdir;
    QFileInfo finfo(QString("%1%2%3").arg(path.absolutePath()).arg(QDir::separator()).arg(pdir));

    if(finfo.isDir()) {
      tryLoadPlugin(finfo.filePath());
    }
  }
}

void
PluginEngine::tryLoadPlugin(QDir path)
{
  qWarning() << "tryLoadPlugin()" << path.absolutePath();

  QStringList files = path.entryList();

  if(!files.contains("info.xml"))
    return;
  
  PluginWrapper *wrapper = new PluginWrapper(QString("%1%2info.xml").arg(path.absolutePath()).arg(QDir::separator()), this);
  wrappers << wrapper;
}

QList<QAction *>
PluginEngine::getAllActions()
{
  QList<QAction *> actions;
  foreach(PluginWrapper *wrapper, wrappers)
    actions << wrapper->menuAction();

  return actions;
}
