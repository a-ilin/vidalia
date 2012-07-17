#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <ControlMethod.h>
#include <QVariant>

#include "qtscriptshell_ControlMethod.h"

static const char * const qtscript_ControlMethod_function_names[] = {
    "ControlMethod"
    // static
    , "fromString"
    , "toString"
    // prototype
};

static const char * const qtscript_ControlMethod_function_signatures[] = {
    ""
    // static
    , "String method"
    , "Method method"
    // prototype
};

static const int qtscript_ControlMethod_function_lengths[] = {
    0
    // static
    , 1
    , 1
    // prototype
};

static QScriptValue qtscript_ControlMethod_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("ControlMethod::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(ControlMethod*)
Q_DECLARE_METATYPE(QtScriptShell_ControlMethod*)
Q_DECLARE_METATYPE(ControlMethod::Method)

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
// ControlMethod::Method
//

static const ControlMethod::Method qtscript_ControlMethod_Method_values[] = {
    ControlMethod::Port
    , ControlMethod::Socket
};

static const char * const qtscript_ControlMethod_Method_keys[] = {
    "Port"
    , "Socket"
};

static QString qtscript_ControlMethod_Method_toStringHelper(ControlMethod::Method value)
{
    if ((value >= ControlMethod::Port) && (value <= ControlMethod::Socket))
        return qtscript_ControlMethod_Method_keys[static_cast<int>(value)-static_cast<int>(ControlMethod::Port)];
    return QString();
}

static QScriptValue qtscript_ControlMethod_Method_toScriptValue(QScriptEngine *engine, const ControlMethod::Method &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("ControlMethod"));
    return clazz.property(qtscript_ControlMethod_Method_toStringHelper(value));
}

static void qtscript_ControlMethod_Method_fromScriptValue(const QScriptValue &value, ControlMethod::Method &out)
{
    out = qvariant_cast<ControlMethod::Method>(value.toVariant());
}

static QScriptValue qtscript_construct_ControlMethod_Method(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= ControlMethod::Port) && (arg <= ControlMethod::Socket))
        return qScriptValueFromValue(engine,  static_cast<ControlMethod::Method>(arg));
    return context->throwError(QString::fromLatin1("Method(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_ControlMethod_Method_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    ControlMethod::Method value = qscriptvalue_cast<ControlMethod::Method>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_ControlMethod_Method_toString(QScriptContext *context, QScriptEngine *engine)
{
    ControlMethod::Method value = qscriptvalue_cast<ControlMethod::Method>(context->thisObject());
    return QScriptValue(engine, qtscript_ControlMethod_Method_toStringHelper(value));
}

static QScriptValue qtscript_create_ControlMethod_Method_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_ControlMethod_Method,
        qtscript_ControlMethod_Method_valueOf, qtscript_ControlMethod_Method_toString);
    qScriptRegisterMetaType<ControlMethod::Method>(engine, qtscript_ControlMethod_Method_toScriptValue,
        qtscript_ControlMethod_Method_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 2; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_ControlMethod_Method_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_ControlMethod_Method_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// ControlMethod
//

static QScriptValue qtscript_ControlMethod_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("ControlMethod(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_ControlMethod* _q_cpp_result = new QtScriptShell_ControlMethod();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue((ControlMethod*)_q_cpp_result));
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        ControlMethod::Method _q_result = ControlMethod::fromString(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        ControlMethod::Method _q_arg0 = qscriptvalue_cast<ControlMethod::Method>(context->argument(0));
        QString _q_result = ControlMethod::toString(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlMethod_throw_ambiguity_error_helper(context,
        qtscript_ControlMethod_function_names[_id],
        qtscript_ControlMethod_function_signatures[_id]);
}

QScriptValue qtscript_create_ControlMethod_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<ControlMethod*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((ControlMethod*)0));

    engine->setDefaultPrototype(qMetaTypeId<ControlMethod*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_ControlMethod_static_call, proto, qtscript_ControlMethod_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 2; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ControlMethod_static_call,
            qtscript_ControlMethod_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_ControlMethod_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("Method"),
        qtscript_create_ControlMethod_Method_class(engine, ctor));
    return ctor;
}
