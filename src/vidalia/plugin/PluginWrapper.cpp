/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file PluginWrapper.cpp
** \brief Wrapper for the plugin scripts
*/

#include "PluginWrapper.h"
#include "PluginEngine.h"
#include "DebugDialog.h"
#include "Vidalia.h"

#include <QtXml>

PluginWrapper::PluginWrapper(const QString &info_path, PluginEngine *engine, QObject *parent)
  : QObject(parent), _engine(engine)
{
  _action = 0;
  _gui = false;
  _persistent = false;
  processInfo(info_path);

  foreach(QString path, _files) {
    DebugDialog::outputDebug(tr("%1: Processing...").arg(name()));

    QFile file(path);
    if(file.open(QIODevice::ReadOnly)) {
      QString contents = file.readAll();
      QScriptSyntaxCheckResult res = QScriptEngine::checkSyntax(contents);
      if(res.state() == QScriptSyntaxCheckResult::Valid) {
        DebugDialog::outputDebug("Everything's ok, evaluating...");
        _engine->evaluate(contents);
        checkExceptions();
      } else {
        DebugDialog::syntaxDebug(tr("%4: ERROR: Line: %1 - Column: %2\n%3")
                                .arg(res.errorLineNumber())
                                .arg(res.errorColumnNumber())
                                .arg(res.errorMessage())
                                .arg(name()));
      }
    } else
      DebugDialog::outputDebug(tr("%1: Error opening file %2")
                                .arg(name()).arg(path));
  }
}

PluginWrapper::~PluginWrapper() {}

void
PluginWrapper::processInfo(const QString &path)
{
  QDomDocument info("Plugin Info");
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)) {
    DebugDialog::outputDebug(tr("%2: ERROR: Cannot open %1")
                              .arg(path).arg(name()));
    return;
  }

  QString errMsg;
  int errLine, errCol;

  if(!info.setContent(&file, false, &errMsg, &errLine, &errCol)) {
    DebugDialog::syntaxDebug(tr("%2: ERROR: Parsing %1")
                            .arg(file.fileName()).arg(name()));
    DebugDialog::syntaxDebug(tr("Line: %1 - Column: %2\nMessage: %3")
                          .arg(errLine).arg(errCol).arg(errMsg));

    file.close();
    return;
  }

  QDomElement root = info.documentElement();

  if(root.tagName() != "VidaliaPlugin") {
    return;
  }

  QDomNode n = root.firstChild();
  while(!n.isNull()) {
    QDomElement e = n.toElement();
    if(!e.isNull()) {
      if(e.tagName() == "name")
        _name = e.text();
      else if(e.tagName() == "author")
        _author = e.text();
      else if(e.tagName() == "date")
        _date = e.text();
      else if(e.tagName() == "type") {
        _persistent = (e.attribute("persistent", "false") == "true");
        _gui = (e.attribute("gui", "false") == "true");
      } else if(e.tagName() == "files") {
        QDomNode froot = e.firstChild();
        while(!froot.isNull()) {
          QDomElement fe = froot.toElement();
          if(fe.tagName() == "file")
            _files << QString("%1%2%3").arg(QFileInfo(path).path()).arg(QDir::separator()).arg(fe.text());
          froot = froot.nextSibling();
        }
      } else if(e.tagName() == "namespace") {
        _nspace = e.text();
      } else if(e.tagName() == "icon") {
        _icon = e.text();
        if(not _icon.startsWith(":"))
          _icon = QFileInfo(path).absoluteDir().canonicalPath() + "/" + _icon;
      }
    }
    n = n.nextSibling();
  }

  file.close();
}

void
PluginWrapper::start()
{
  DebugDialog::outputDebug(tr("%1: Starting...").arg(name()));
  _engine->evaluate(QString("%1.start()").arg(nspace()));
  checkExceptions();
}

void
PluginWrapper::stop()
{
  DebugDialog::outputDebug(tr("%1: Stoping...").arg(name()));
  _engine->evaluate(QString("%1.stop()").arg(nspace()));
  checkExceptions();
}

VidaliaTab *
PluginWrapper::buildGUI()
{
  if(!hasGUI()) {
    DebugDialog::outputDebug(tr("%1: WARNING: doesn't have a GUI, and buildGUI() was called")
                              .arg(name()));
    if(checkExceptions())
      return NULL;
  }

  VidaliaTab *tab = qscriptvalue_cast<VidaliaTab *>(
      _engine->evaluate(QString("%1.buildGUI()").arg(nspace())));

  if(checkExceptions())
    return NULL;

  return tab;
}

bool
PluginWrapper::checkExceptions()
{
  if(_engine->hasUncaughtException()) {
    DebugDialog::exceptDebug(tr("%2:\n*** Exception in line %1")
                             .arg(_engine->uncaughtExceptionLineNumber())
                             .arg(_engine->uncaughtException().toString()));
    DebugDialog::exceptDebug(tr("*** Backtrace:"));
    foreach(QString line, _engine->uncaughtExceptionBacktrace()) {
      vInfo(line);
      DebugDialog::exceptDebug(line);
    }

    _engine->clearExceptions();

    return true;
  }

  return false;
}

bool
PluginWrapper::hasGUI()
{
  return _gui;
}

bool
PluginWrapper::isPersistent()
{
  return _persistent;
}

QString
PluginWrapper::name() const
{
  if(_name.isEmpty())
    return tr("(untitled)");
  return _name;
}

QString
PluginWrapper::date() const
{
  return _date;
}

QString
PluginWrapper::author() const
{
  return _author;
}

QString
PluginWrapper::nspace() const
{
  return _nspace;
}

QStringList
PluginWrapper::files() const
{
  return _files;
}

QString
PluginWrapper::icon() const
{
  return _icon;
}

QAction *
PluginWrapper::menuAction()
{
  if (not _icon.isEmpty())
    _action = new QAction(QIcon(_icon), _name, this);
  else
    _action = new QAction(_name, this);

  if(hasGUI()) {
    connect(_action, SIGNAL(triggered()), this, SLOT(emitPluginTab()));
  }

  // if it hasn't been started yet
  if(!isPersistent()) {
    connect(_action, SIGNAL(triggered()), this, SLOT(start()));
  }

  return _action;
}

void
PluginWrapper::emitPluginTab()
{
  VidaliaTab *tab = buildGUI();
  if(tab)
    emit pluginTab(tab);
  else
    DebugDialog::exceptDebug(tr("Error: buildGUI() failed for plugin %1")
                            .arg(name()));
}
