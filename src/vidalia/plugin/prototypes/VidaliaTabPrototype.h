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
** \file VidaliaTabPrototype.h
** \brief Prototype for VidaliaTab class
*/

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

