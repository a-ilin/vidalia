#ifndef VIDALIATABPROT_H
#define VIDALIATABPROT_H

#include <QtGui>
#include <QtScript>

#include "VidaliaTab.h"

class VidaliaTabPrototype : public QObject, public QScriptable
{
  Q_OBJECT

  public:
    VidaliaTabPrototype(QObject *parent = 0);
    static QScriptValue constructor(QScriptContext *context, QScriptEngine *engine);
    static int metaTypeId();
    static QString name();

    Q_INVOKABLE void setLayout(QLayout *layout);
    Q_INVOKABLE QVariant getSetting(QString name, QVariant defaultValue);
    Q_INVOKABLE void saveSetting(QString name, QVariant value);
};

Q_DECLARE_METATYPE(VidaliaTab *);

#endif

