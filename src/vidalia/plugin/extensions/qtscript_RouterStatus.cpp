#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <RouterStatus.h>
#include <QVariant>
#include <qdatetime.h>
#include <qhostaddress.h>
#include <qstringlist.h>

static const char * const qtscript_RouterStatus_function_names[] = {
    "RouterStatus"
    // static
    // prototype
    , "bandwidth"
    , "descriptorDigest"
    , "dirPort"
    , "id"
    , "ipAddress"
    , "isRunning"
    , "isValid"
    , "name"
    , "orPort"
    , "published"
    , "toString"
};

static const char * const qtscript_RouterStatus_function_signatures[] = {
    "List status"
    // static
    // prototype
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
""
};

static const int qtscript_RouterStatus_function_lengths[] = {
    1
    // static
    // prototype
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
};

static QScriptValue qtscript_RouterStatus_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("RouterStatus::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(RouterStatus*)
Q_DECLARE_METATYPE(RouterStatus::Flag)
Q_DECLARE_METATYPE(QHostAddress)

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
// RouterStatus::Flag
//

static const RouterStatus::Flag qtscript_RouterStatus_Flag_values[] = {
    RouterStatus::Unknown
    , RouterStatus::Authority
    , RouterStatus::BadExit
    , RouterStatus::BadDirectory
    , RouterStatus::Exit
    , RouterStatus::Fast
    , RouterStatus::Guard
    , RouterStatus::HSDir
    , RouterStatus::Named
    , RouterStatus::Stable
    , RouterStatus::Running
    , RouterStatus::Valid
    , RouterStatus::V2Dir
    , RouterStatus::V3Dir
};

static const char * const qtscript_RouterStatus_Flag_keys[] = {
    "Unknown"
    , "Authority"
    , "BadExit"
    , "BadDirectory"
    , "Exit"
    , "Fast"
    , "Guard"
    , "HSDir"
    , "Named"
    , "Stable"
    , "Running"
    , "Valid"
    , "V2Dir"
    , "V3Dir"
};

static QString qtscript_RouterStatus_Flag_toStringHelper(RouterStatus::Flag value)
{
    for (int i = 0; i < 14; ++i) {
        if (qtscript_RouterStatus_Flag_values[i] == value)
            return QString::fromLatin1(qtscript_RouterStatus_Flag_keys[i]);
    }
    return QString();
}

static QScriptValue qtscript_RouterStatus_Flag_toScriptValue(QScriptEngine *engine, const RouterStatus::Flag &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("RouterStatus"));
    return clazz.property(qtscript_RouterStatus_Flag_toStringHelper(value));
}

static void qtscript_RouterStatus_Flag_fromScriptValue(const QScriptValue &value, RouterStatus::Flag &out)
{
    out = qvariant_cast<RouterStatus::Flag>(value.toVariant());
}

static QScriptValue qtscript_construct_RouterStatus_Flag(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    for (int i = 0; i < 14; ++i) {
        if (qtscript_RouterStatus_Flag_values[i] == arg)
            return qScriptValueFromValue(engine,  static_cast<RouterStatus::Flag>(arg));
    }
    return context->throwError(QString::fromLatin1("Flag(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_RouterStatus_Flag_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    RouterStatus::Flag value = qscriptvalue_cast<RouterStatus::Flag>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_RouterStatus_Flag_toString(QScriptContext *context, QScriptEngine *engine)
{
    RouterStatus::Flag value = qscriptvalue_cast<RouterStatus::Flag>(context->thisObject());
    return QScriptValue(engine, qtscript_RouterStatus_Flag_toStringHelper(value));
}

static QScriptValue qtscript_create_RouterStatus_Flag_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_RouterStatus_Flag,
        qtscript_RouterStatus_Flag_valueOf, qtscript_RouterStatus_Flag_toString);
    qScriptRegisterMetaType<RouterStatus::Flag>(engine, qtscript_RouterStatus_Flag_toScriptValue,
        qtscript_RouterStatus_Flag_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 14; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_RouterStatus_Flag_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_RouterStatus_Flag_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// RouterStatus
//

static QScriptValue qtscript_RouterStatus_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 10;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    RouterStatus* _q_self = qscriptvalue_cast<RouterStatus*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("RouterStatus.%0(): this object is not a RouterStatus")
            .arg(qtscript_RouterStatus_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        unsigned long long _q_result = _q_self->bandwidth();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->descriptorDigest();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        unsigned short _q_result = _q_self->dirPort();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->id();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        QHostAddress _q_result = _q_self->ipAddress();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isRunning();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isValid();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->name();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 8:
    if (context->argumentCount() == 0) {
        unsigned short _q_result = _q_self->orPort();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 9:
    if (context->argumentCount() == 0) {
        QDateTime _q_result = _q_self->published();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 10: {
    QString result = QString::fromLatin1("RouterStatus");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_RouterStatus_throw_ambiguity_error_helper(context,
        qtscript_RouterStatus_function_names[_id+1],
        qtscript_RouterStatus_function_signatures[_id+1]);
}

static QScriptValue qtscript_RouterStatus_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("RouterStatus(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 1) {
        QStringList _q_arg0;
        qScriptValueToSequence(context->argument(0), _q_arg0);
        RouterStatus* _q_cpp_result = new RouterStatus(_q_arg0);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_RouterStatus_throw_ambiguity_error_helper(context,
        qtscript_RouterStatus_function_names[_id],
        qtscript_RouterStatus_function_signatures[_id]);
}

QScriptValue qtscript_create_RouterStatus_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<RouterStatus*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((RouterStatus*)0));
    for (int i = 0; i < 11; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_RouterStatus_prototype_call, qtscript_RouterStatus_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_RouterStatus_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<RouterStatus*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_RouterStatus_static_call, proto, qtscript_RouterStatus_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    ctor.setProperty(QString::fromLatin1("Flag"),
        qtscript_create_RouterStatus_Flag_class(engine, ctor));
    return ctor;
}
