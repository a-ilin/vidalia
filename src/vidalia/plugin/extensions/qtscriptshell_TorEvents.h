#ifndef QTSCRIPTSHELL_TOREVENTS_H
#define QTSCRIPTSHELL_TOREVENTS_H

#include <TorEvents.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_TorEvents : public TorEvents
{
public:
    QtScriptShell_TorEvents(QObject*  parent = 0);
    ~QtScriptShell_TorEvents();

    void childEvent(QChildEvent*  arg__1);
    void customEvent(QEvent*  arg__1);
    bool  event(QEvent*  arg__1);
    bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
    void timerEvent(QTimerEvent*  arg__1);

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_TOREVENTS_H
