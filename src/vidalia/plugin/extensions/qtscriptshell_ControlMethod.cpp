#include "qtscriptshell_ControlMethod.h"

#include <QtScript/QScriptEngine>
#include <QVariant>

#define QTSCRIPT_IS_GENERATED_FUNCTION(fun) ((fun.data().toUInt32() & 0xFFFF0000) == 0xBABE0000)


QtScriptShell_ControlMethod::QtScriptShell_ControlMethod()
    : ControlMethod() {}

QtScriptShell_ControlMethod::~QtScriptShell_ControlMethod() {}

