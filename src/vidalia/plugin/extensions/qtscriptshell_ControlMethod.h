#ifndef QTSCRIPTSHELL_CONTROLMETHOD_H
#define QTSCRIPTSHELL_CONTROLMETHOD_H

#include <ControlMethod.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_ControlMethod : public ControlMethod
{
public:
    QtScriptShell_ControlMethod();
    ~QtScriptShell_ControlMethod();


    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_CONTROLMETHOD_H
