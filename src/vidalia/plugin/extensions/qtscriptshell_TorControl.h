#ifndef QTSCRIPTSHELL_TORCONTROL_H
#define QTSCRIPTSHELL_TORCONTROL_H

#include <TorControl.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_TorControl : public TorControl
{
public:
    QtScriptShell_TorControl(ControlMethod::Method  method = ControlMethod::Port);
    ~QtScriptShell_TorControl();

    void childEvent(QChildEvent*  arg__1);
    void customEvent(QEvent*  arg__1);
    bool  event(QEvent*  arg__1);
    bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
    void timerEvent(QTimerEvent*  arg__1);

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_TORCONTROL_H
