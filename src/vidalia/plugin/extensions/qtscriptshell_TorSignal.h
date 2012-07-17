#ifndef QTSCRIPTSHELL_TORSIGNAL_H
#define QTSCRIPTSHELL_TORSIGNAL_H

#include <TorSignal.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_TorSignal : public TorSignal
{
public:
    QtScriptShell_TorSignal();
    ~QtScriptShell_TorSignal();


    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_TORSIGNAL_H
