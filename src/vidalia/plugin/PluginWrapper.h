#ifndef PLUGINWRAPPER_H
#define PLUGINWRAPPER_H

#include <QtCore>

#include "VidaliaTab.h"

class PluginEngine;

class PluginWrapper : public QObject {
  Q_OBJECT

  public:
    PluginWrapper(const QString &info_path, PluginEngine *engine, QObject *parent = 0);
    ~PluginWrapper();

    bool hasGUI();
    bool isPersistent();

    QString name() const;
    QString date() const;
    QString author() const;
    QString nspace() const;
    QStringList files() const;

    QAction *menuAction();

  signals:
    void pluginTab(VidaliaTab *);

  public slots:
    void start();
    void stop();
    VidaliaTab *buildGUI();

  private slots:
    void emitPluginTab();

  protected:
    void processInfo(const QString &path);
    bool checkExceptions();

    PluginEngine *_engine;
    QString _name, _date, _author;
    bool _persistent, _gui;
    QStringList _files;
    QString _nspace;

    QAction *_action;
};

#endif

