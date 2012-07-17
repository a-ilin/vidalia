#ifndef QTSCRIPTSHELL_CONTROLSOCKET_H
#define QTSCRIPTSHELL_CONTROLSOCKET_H

#include <ControlSocket.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_ControlSocket : public ControlSocket
{
public:
    QtScriptShell_ControlSocket(ControlMethod::Method  method = ControlMethod::Port);
    ~QtScriptShell_ControlSocket();

    void childEvent(QChildEvent*  arg__1);
    void customEvent(QEvent*  event);
    bool  event(QEvent*  arg__1);
    bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
    void timerEvent(QTimerEvent*  arg__1);

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_CONTROLSOCKET_H
