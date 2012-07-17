#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <TorProcess.h>
#include <QVariant>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qlist.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringlist.h>

#include "qtscriptshell_TorProcess.h"

static const char * const qtscript_TorProcess_function_names[] = {
    "TorProcess"
    // static
    , "version"
    // prototype
    , "closeStdout"
    , "openStdout"
    , "pid"
    , "start"
    , "stop"
    , "toString"
};

static const char * const qtscript_TorProcess_function_signatures[] = {
    "QObject parent"
    // static
    , "String exe"
    // prototype
    , ""
    , ""
    , ""
    , "String app, List args"
    , "String errmsg"
""
};

static const int qtscript_TorProcess_function_lengths[] = {
    1
    // static
    , 1
    // prototype
    , 0
    , 0
    , 0
    , 2
    , 1
    , 0
};

static QScriptValue qtscript_TorProcess_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("TorProcess::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(TorProcess*)
Q_DECLARE_METATYPE(QtScriptShell_TorProcess*)
Q_DECLARE_METATYPE(QString*)
Q_DECLARE_METATYPE(QProcess*)

//
// TorProcess
//

static QScriptValue qtscript_TorProcess_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 5;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    TorProcess* _q_self = qscriptvalue_cast<TorProcess*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("TorProcess.%0(): this object is not a TorProcess")
            .arg(qtscript_TorProcess_function_names[_id+2]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        _q_self->closeStdout();
        return context->engine()->undefinedValue();
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        _q_self->openStdout();
        return context->engine()->undefinedValue();
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        unsigned long long _q_result = _q_self->pid();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QStringList _q_arg1;
        qScriptValueToSequence(context->argument(1), _q_arg1);
        _q_self->start(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->stop();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->stop(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5: {
    QString result = QString::fromLatin1("TorProcess");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorProcess_throw_ambiguity_error_helper(context,
        qtscript_TorProcess_function_names[_id+2],
        qtscript_TorProcess_function_signatures[_id+2]);
}

static QScriptValue qtscript_TorProcess_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("TorProcess(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_TorProcess* _q_cpp_result = new QtScriptShell_TorProcess();
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (TorProcess*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QObject* _q_arg0 = context->argument(0).toQObject();
        QtScriptShell_TorProcess* _q_cpp_result = new QtScriptShell_TorProcess(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (TorProcess*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        QString _q_result = TorProcess::version(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorProcess_throw_ambiguity_error_helper(context,
        qtscript_TorProcess_function_names[_id],
        qtscript_TorProcess_function_signatures[_id]);
}

static QScriptValue qtscript_TorProcess_toScriptValue(QScriptEngine *engine, TorProcess* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_TorProcess_fromScriptValue(const QScriptValue &value, TorProcess* &out)
{
    out = qobject_cast<TorProcess*>(value.toQObject());
}

QScriptValue qtscript_create_TorProcess_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<TorProcess*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((TorProcess*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QProcess*>()));
    for (int i = 0; i < 6; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_TorProcess_prototype_call, qtscript_TorProcess_function_lengths[i+2]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_TorProcess_function_names[i+2]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<TorProcess*>(engine, qtscript_TorProcess_toScriptValue, 
        qtscript_TorProcess_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_TorProcess_static_call, proto, qtscript_TorProcess_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 1; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_TorProcess_static_call,
            qtscript_TorProcess_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_TorProcess_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    return ctor;
}
