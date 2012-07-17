#ifndef QTSCRIPTSHELL_CONTROLCONNECTION_H
#define QTSCRIPTSHELL_CONTROLCONNECTION_H

#include <ControlConnection.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_ControlConnection : public ControlConnection
{
public:
    QtScriptShell_ControlConnection(ControlMethod::Method  method, TorEvents*  events = 0);
    ~QtScriptShell_ControlConnection();

    void childEvent(QChildEvent*  arg__1);
    void customEvent(QEvent*  arg__1);
    bool  event(QEvent*  arg__1);
    bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
    void timerEvent(QTimerEvent*  arg__1);

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_CONTROLCONNECTION_H
