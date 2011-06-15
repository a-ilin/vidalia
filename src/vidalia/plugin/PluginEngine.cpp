#include "PluginEngine.h"
#include "VidaliaSettings.h"
#include "PluginWrapper.h"
#include "DebugDialog.h"

#include "Vidalia.h"

PluginEngine::PluginEngine(QObject *parent)
  : QScriptEngine(parent)
{
  ADD_PROTOTYPE(VidaliaTabPrototype)
  MAKE_CREATABLE(VidaliaTabPrototype)
  ADD_PROTOTYPE(HelperProcessPrototype)
  MAKE_CREATABLE(HelperProcessPrototype)
  ADD_PROTOTYPE(TorControlPrototype)

  globalObject().setProperty("torControl", newQObject(Vidalia::torControl()));

  globalObject().setProperty("include", newFunction(includeScript));
  globalObject().setProperty("importExtension", newFunction(importExtension));
  globalObject().setProperty("vdebug", newFunction(vdebug));

  VidaliaSettings settings;
  globalObject().setProperty("pluginPath", QScriptValue(settings.pluginPath()));

  DebugDialog::outputDebug("Available extensions:");
  foreach(QString ext, availableExtensions())
    DebugDialog::outputDebug(QString("  %1").arg(ext));

  loadAllPlugins();
}

PluginEngine::~PluginEngine() 
{
  foreach(PluginWrapper *wrapper, wrappers)
    wrapper->stop();
}

void
PluginEngine::loadAllPlugins()
{
  DebugDialog::outputDebug("Loading all plugins...");

  VidaliaSettings settings;
  QDir path = QDir(settings.pluginPath());

  DebugDialog::outputDebug(QString("PluginPath=%1").arg(path.absolutePath()));
  
  foreach(QString pdir, path.entryList(QDir::NoDotAndDotDot|QDir::AllDirs)) {
    QFileInfo finfo(QString("%1%2%3")
        .arg(path.absolutePath())
        .arg(QDir::separator())
        .arg(pdir));

    if(finfo.isDir()) {
      tryLoadPlugin(finfo.filePath());
    }
  }
}

void
PluginEngine::tryLoadPlugin(QDir path)
{
  QStringList files = path.entryList();

  if(!files.contains("info.xml")) {
    DebugDialog::outputDebug(tr("WARNING: %1 doesn't have an info file.")
        .arg(path.absolutePath()));
    return;
  }
  
  PluginWrapper *wrapper = new PluginWrapper(QString("%1%2info.xml")
                                            .arg(path.absolutePath())
                                            .arg(QDir::separator()), this);

  // if it's persistent, start it right away
  if(wrapper->isPersistent()) {
    wrapper->start();
  }

  wrappers << wrapper;

  connect(wrapper, SIGNAL(pluginTab(VidaliaTab *)), this, SIGNAL(pluginTab(VidaliaTab *)));
}

QList<QAction *>
PluginEngine::getAllActions()
{
  QList<QAction *> actions;
  foreach(PluginWrapper *wrapper, wrappers)
    actions << wrapper->menuAction();

  return actions;
}

QScriptValue 
PluginEngine::importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}

QScriptValue 
PluginEngine::includeScript(QScriptContext *context, QScriptEngine *engine)
{
  QString currentFileName = engine->globalObject().property("qs").property("script").property("absoluteFilePath").toString();
  QFileInfo currentFileInfo(currentFileName);
  QString path = currentFileInfo.path();
  QString importFile = context->argument(0).toString();
  QFileInfo importInfo(importFile);
  if (importInfo.isRelative()) {
      importFile =  path + "/" + importInfo.filePath();
  }
  if (!loadFile(importFile, engine)) {
      return context->throwError(QString("Failed to resolve include: %1").arg(importFile));
  }
  return engine->toScriptValue(true);
}

bool 
PluginEngine::loadFile(QString fileName, QScriptEngine *engine)
{
    // avoid loading files more than once
    static QSet<QString> loadedFiles;
    QFileInfo fileInfo(fileName);
    QString absoluteFileName = fileInfo.absoluteFilePath();
    QString absolutePath = fileInfo.absolutePath();
    QString canonicalFileName = fileInfo.canonicalFilePath();
    if (loadedFiles.contains(canonicalFileName)) {
        return true;
    }
    loadedFiles.insert(canonicalFileName);
    QString path = fileInfo.path();

    // load the file
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString contents = stream.readAll();
        file.close();

        int endlineIndex = contents.indexOf('\n');
        QString line = contents.left(endlineIndex);
        int lineNumber = 1;

        // strip off #!/usr/bin/env qscript line
        if (line.startsWith("#!")) {
            contents.remove(0, endlineIndex+1);
            ++lineNumber;
        }

        // set qt.script.absoluteFilePath
        QScriptValue script = engine->globalObject().property("qs").property("script");
        QScriptValue oldFilePathValue = script.property("absoluteFilePath");
        QScriptValue oldPathValue = script.property("absolutePath");
        script.setProperty("absoluteFilePath", engine->toScriptValue(absoluteFileName));
        script.setProperty("absolutePath", engine->toScriptValue(absolutePath));

        QScriptValue r = engine->evaluate(contents, fileName, lineNumber);
        if (engine->hasUncaughtException()) {
            QStringList backtrace = engine->uncaughtExceptionBacktrace();
            qDebug() << QString("    %1\n%2\n\n").arg(r.toString()).arg(backtrace.join("\n"));
            return true;
        }
        script.setProperty("absoluteFilePath", oldFilePathValue); // if we come from includeScript(), or whereever
        script.setProperty("absolutePath", oldPathValue); // if we come from includeScript(), or whereever
    } else {
        return false;
    }
    return true;
}

QScriptValue
PluginEngine::vdebug(QScriptContext *context, QScriptEngine *engine)
{
  QString result;
  for(int i = 0; i<context->argumentCount(); i++) {
    if(i>0)
      result.append(" ");
    result.append(context->argument(i).toString());
  }

  qWarning() << result;

  return engine->undefinedValue();
}
