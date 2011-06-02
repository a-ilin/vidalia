#include "PluginWrapper.h"
#include "PluginEngine.h"

#include <QtXml>

PluginWrapper::PluginWrapper(const QString &info_path, PluginEngine *engine, QObject *parent)
  : QObject(parent), _engine(engine)
{
  _action = 0;
  _gui = false;
  _persistent = false;
  processInfo(info_path);

  foreach(QString path, _files) {
    qWarning() << path;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly)) {
      _engine->evaluate(file.readAll());
      qWarning() << "evaluated";
    } else 
      qWarning() << "Error opening file";
  }
}

PluginWrapper::~PluginWrapper() {}

void
PluginWrapper::processInfo(const QString &path)
{
  qWarning() << "processInfo()";

  QDomDocument info("Plugin Info");
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Problem opening xml file";
    return;
  }

  if(!info.setContent(&file)) {
    qWarning() << "Problem setting contents";
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
      }
    }
    n = n.nextSibling();
  }

  file.close();
}

void
PluginWrapper::start()
{
  _engine->evaluate(QString("%1.start()").arg(nspace()));
}

void
PluginWrapper::stop()
{
  _engine->evaluate(QString("%1.stop()").arg(nspace()));
}

VidaliaTab *
PluginWrapper::buildGUI()
{
  if(!hasGUI())
    return NULL;
  VidaliaTab *tab = qscriptvalue_cast<VidaliaTab *>(_engine->evaluate(QString("%1.buildGUI()").arg(nspace())));
  if(_engine->hasUncaughtException()) {
    qWarning() << "Exception:";
    qWarning() << _engine->uncaughtExceptionLineNumber();

    return NULL;
  }
  qWarning() << "Casted tab:" << tab << nspace();
  return tab;
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

QAction *
PluginWrapper::menuAction()
{
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
    // TODO: make this more than a console print
    qWarning() << "Error: buildGUI() failed for plugin" << name();
}
