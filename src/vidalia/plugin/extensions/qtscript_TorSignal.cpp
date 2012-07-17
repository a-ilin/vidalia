#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <TorSignal.h>
#include <QVariant>

#include "qtscriptshell_TorSignal.h"

static const char * const qtscript_TorSignal_function_names[] = {
    "TorSignal"
    // static
    , "toString"
    // prototype
};

static const char * const qtscript_TorSignal_function_signatures[] = {
    ""
    // static
    , "Signal signal"
    // prototype
};

static const int qtscript_TorSignal_function_lengths[] = {
    0
    // static
    , 1
    // prototype
};

static QScriptValue qtscript_TorSignal_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("TorSignal::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(TorSignal*)
Q_DECLARE_METATYPE(QtScriptShell_TorSignal*)
Q_DECLARE_METATYPE(TorSignal::Signal)

static QScriptValue qtscript_create_enum_class_helper(
    QScriptEngine *engine,
    QScriptEngine::FunctionSignature construct,
    QScriptEngine::FunctionSignature valueOf,
    QScriptEngine::FunctionSignature toString)
{
    QScriptValue proto = engine->newObject();
    proto.setProperty(QString::fromLatin1("valueOf"),
        engine->newFunction(valueOf), QScriptValue::SkipInEnumeration);
    proto.setProperty(QString::fromLatin1("toString"),
        engine->newFunction(toString), QScriptValue::SkipInEnumeration);
    return engine->newFunction(construct, proto, 1);
}

//
// TorSignal::Signal
//

static const TorSignal::Signal qtscript_TorSignal_Signal_values[] = {
    TorSignal::Reload
    , TorSignal::Shutdown
    , TorSignal::Dump
    , TorSignal::Debug
    , TorSignal::Halt
    , TorSignal::NewNym
};

static const char * const qtscript_TorSignal_Signal_keys[] = {
    "Reload"
    , "Shutdown"
    , "Dump"
    , "Debug"
    , "Halt"
    , "NewNym"
};

static QString qtscript_TorSignal_Signal_toStringHelper(TorSignal::Signal value)
{
    if ((value >= TorSignal::Reload) && (value <= TorSignal::NewNym))
        return qtscript_TorSignal_Signal_keys[static_cast<int>(value)-static_cast<int>(TorSignal::Reload)];
    return QString();
}

static QScriptValue qtscript_TorSignal_Signal_toScriptValue(QScriptEngine *engine, const TorSignal::Signal &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("TorSignal"));
    return clazz.property(qtscript_TorSignal_Signal_toStringHelper(value));
}

static void qtscript_TorSignal_Signal_fromScriptValue(const QScriptValue &value, TorSignal::Signal &out)
{
    out = qvariant_cast<TorSignal::Signal>(value.toVariant());
}

static QScriptValue qtscript_construct_TorSignal_Signal(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= TorSignal::Reload) && (arg <= TorSignal::NewNym))
        return qScriptValueFromValue(engine,  static_cast<TorSignal::Signal>(arg));
    return context->throwError(QString::fromLatin1("Signal(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_TorSignal_Signal_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    TorSignal::Signal value = qscriptvalue_cast<TorSignal::Signal>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_TorSignal_Signal_toString(QScriptContext *context, QScriptEngine *engine)
{
    TorSignal::Signal value = qscriptvalue_cast<TorSignal::Signal>(context->thisObject());
    return QScriptValue(engine, qtscript_TorSignal_Signal_toStringHelper(value));
}

static QScriptValue qtscript_create_TorSignal_Signal_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_TorSignal_Signal,
        qtscript_TorSignal_Signal_valueOf, qtscript_TorSignal_Signal_toString);
    qScriptRegisterMetaType<TorSignal::Signal>(engine, qtscript_TorSignal_Signal_toScriptValue,
        qtscript_TorSignal_Signal_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 6; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_TorSignal_Signal_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_TorSignal_Signal_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// TorSignal
//

static QScriptValue qtscript_TorSignal_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("TorSignal(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_TorSignal* _q_cpp_result = new QtScriptShell_TorSignal();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue((TorSignal*)_q_cpp_result));
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        TorSignal::Signal _q_arg0 = qscriptvalue_cast<TorSignal::Signal>(context->argument(0));
        QString _q_result = TorSignal::toString(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorSignal_throw_ambiguity_error_helper(context,
        qtscript_TorSignal_function_names[_id],
        qtscript_TorSignal_function_signatures[_id]);
}

QScriptValue qtscript_create_TorSignal_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<TorSignal*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((TorSignal*)0));

    engine->setDefaultPrototype(qMetaTypeId<TorSignal*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_TorSignal_static_call, proto, qtscript_TorSignal_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 1; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_TorSignal_static_call,
            qtscript_TorSignal_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_TorSignal_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("Signal"),
        qtscript_create_TorSignal_Signal_class(engine, ctor));
    return ctor;
}
