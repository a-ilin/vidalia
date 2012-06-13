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
** \file PluginEngine.cpp
** \brief Engine that handles all plugin related features
*/

#include "PluginEngine.h"
#include "VidaliaSettings.h"
#include "PluginWrapper.h"
#include "DebugDialog.h"

#include "Vidalia.h"
#include "VidaliaExtensions.h"

#include <unistd.h>

PluginEngine::PluginEngine(QObject *parent)
  : QScriptEngine(parent)
{
  VidaliaExtensions ve;
  ve.initialize("vidalia", this);

  ADD_PROTOTYPE(VidaliaTabPrototype)
  MAKE_CREATABLE(VidaliaTabPrototype)
  ADD_PROTOTYPE(HelperProcessPrototype)
  MAKE_CREATABLE(HelperProcessPrototype)
  ADD_PROTOTYPE(TorrcPrototype)

  globalObject().setProperty("torControl", newQObject(Vidalia::torControl()));
  globalObject().setProperty("torrc", newQObject(Vidalia::torrc()));
  globalObject().setProperty("vidaliaApp", newQObject(vApp));

//  globalObject().setProperty("include", newFunction(includeScript));
  globalObject().setProperty("importExtension", newFunction(importExtension));
  globalObject().setProperty("vdebug", newFunction(vdebug));
  globalObject().setProperty("findWidget", newFunction(findWidget));
  globalObject().setProperty("sleep", newFunction(sleep));

  VidaliaSettings settings;
  QDir path(settings.pluginPath());
  if(path.isRelative()) {
    path.setPath(QApplication::applicationDirPath() + "/" + settings.pluginPath());
  }

  globalObject().setProperty("pluginPath", QScriptValue(path.canonicalPath()));

  DebugDialog::outputDebug("Available extensions:");
  foreach(QString ext, availableExtensions())
    DebugDialog::outputDebug(QString("  %1").arg(ext));
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
  QDir path(settings.pluginPath());
  if(path.isRelative()) {
    path.setPath(QApplication::applicationDirPath() + "/" + settings.pluginPath());
  }

  DebugDialog::outputDebug(QString("PluginPath=%1").arg(path.canonicalPath()));

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
PluginEngine::provide(const QString &name, QObject *obj)
{
  globalObject().setProperty(name, newQObject(obj));
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

//QScriptValue
//PluginEngine::includeScript(QScriptContext *context, QScriptEngine *engine)
//{
//  VidaliaSettings settings;
//  QString path = settings.pluginPath();
//  QString importFile = context->argument(0).toString();
//  QFileInfo importInfo(importFile);
//  if (importInfo.isRelative()) {
//      importFile =  path + "/" + importInfo.filePath();
//  }

//  if (!loadFile(importFile, engine)) {
//      return context->throwError(QString("Failed to resolve include: %1").arg(importFile));
//  }
//  return engine->toScriptValue(true);
//}

//bool
//PluginEngine::loadFile(QString fileName, QScriptEngine *engine)
//{
//    static QSet<QString> loadedFiles;
//    QFileInfo fileInfo(fileName);
//    QString absoluteFileName = fileInfo.absoluteFilePath();
//    QString absolutePath = fileInfo.absolutePath();
//    QString canonicalFileName = fileInfo.canonicalFilePath();
//    if (loadedFiles.contains(canonicalFileName)) {
//        return true;
//    }
//    loadedFiles.insert(canonicalFileName);
//    QString path = fileInfo.path();

//    QFile file(fileName);
//    if (file.open(QFile::ReadOnly)) {
//        QTextStream stream(&file);
//        QString contents = stream.readAll();
//        file.close();

//        QScriptValue r = engine->evaluate(contents);
//        if (engine->hasUncaughtException()) {
//            QStringList backtrace = engine->uncaughtExceptionBacktrace();
//            qDebug() << QString("    %1\n%2\n\n").arg(r.toString()).arg(backtrace.join("\n"));
//            return true;
//        }
//    } else {
//        return false;
//    }
//    return true;
//}

QScriptValue
PluginEngine::vdebug(QScriptContext *context, QScriptEngine *engine)
{
  QString result;
  for(int i = 0; i<context->argumentCount(); i++) {
    if(i>0)
      result.append(" ");
    result.append(context->argument(i).toString());
  }

  vInfo(result);

  return engine->undefinedValue();
}

QScriptValue
PluginEngine::findWidget(QScriptContext *context, QScriptEngine *engine)
{
  if(context->argumentCount() != 2)
    return context->throwError(QString("findWidget called with the wrong argument count. Expected 2."));

  QWidget *widget = qscriptvalue_cast<QWidget *>(context->argument(0));
  QString name = context->argument(1).toString();

  QObjectList list = widget->children();
  QScriptValue ret = engine->nullValue();

  for(int i = 0; i < list.length(); i++) {
    if(list[i]->objectName() == name) {
      ret = QScriptValue(i);
      break;
    }
  }

  return ret;
}

QScriptValue
PluginEngine::sleep(QScriptContext *context, QScriptEngine *engine)
{
  if(context->argumentCount() != 1)
    return context->throwError(QString("sleep must be called with 1 parameter."));

  int s = context->argument(1).toInt32();

#if defined(Q_WS_WIN)
  Sleep(s*1000);
#else
  (void)::sleep(s);
#endif

  return engine->nullValue();
}
