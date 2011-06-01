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
};

Q_DECLARE_METATYPE(VidaliaTab *);

#endif

