#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <BootstrapStatus.h>
#include <QVariant>

static const char * const qtscript_BootstrapStatus_function_names[] = {
    "BootstrapStatus"
    // static
    , "actionFromString"
    , "statusFromString"
    // prototype
    , "description"
    , "isValid"
    , "percentComplete"
    , "recommendedAction"
    , "status"
    , "warning"
    , "toString"
};

static const char * const qtscript_BootstrapStatus_function_signatures[] = {
    ""
    // static
    , "String str"
    , "String tag"
    // prototype
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
""
};

static const int qtscript_BootstrapStatus_function_lengths[] = {
    0
    // static
    , 1
    , 1
    // prototype
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
};

static QScriptValue qtscript_BootstrapStatus_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("BootstrapStatus::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(BootstrapStatus*)
Q_DECLARE_METATYPE(BootstrapStatus::Recommendation)
Q_DECLARE_METATYPE(BootstrapStatus::Status)

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
// BootstrapStatus::Recommendation
//

static const BootstrapStatus::Recommendation qtscript_BootstrapStatus_Recommendation_values[] = {
    BootstrapStatus::UnrecognizedRecommendation
    , BootstrapStatus::RecommendIgnore
    , BootstrapStatus::RecommendWarn
};

static const char * const qtscript_BootstrapStatus_Recommendation_keys[] = {
    "UnrecognizedRecommendation"
    , "RecommendIgnore"
    , "RecommendWarn"
};

static QString qtscript_BootstrapStatus_Recommendation_toStringHelper(BootstrapStatus::Recommendation value)
{
    if ((value >= BootstrapStatus::UnrecognizedRecommendation) && (value <= BootstrapStatus::RecommendWarn))
        return qtscript_BootstrapStatus_Recommendation_keys[static_cast<int>(value)-static_cast<int>(BootstrapStatus::UnrecognizedRecommendation)];
    return QString();
}

static QScriptValue qtscript_BootstrapStatus_Recommendation_toScriptValue(QScriptEngine *engine, const BootstrapStatus::Recommendation &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("BootstrapStatus"));
    return clazz.property(qtscript_BootstrapStatus_Recommendation_toStringHelper(value));
}

static void qtscript_BootstrapStatus_Recommendation_fromScriptValue(const QScriptValue &value, BootstrapStatus::Recommendation &out)
{
    out = qvariant_cast<BootstrapStatus::Recommendation>(value.toVariant());
}

static QScriptValue qtscript_construct_BootstrapStatus_Recommendation(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= BootstrapStatus::UnrecognizedRecommendation) && (arg <= BootstrapStatus::RecommendWarn))
        return qScriptValueFromValue(engine,  static_cast<BootstrapStatus::Recommendation>(arg));
    return context->throwError(QString::fromLatin1("Recommendation(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_BootstrapStatus_Recommendation_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    BootstrapStatus::Recommendation value = qscriptvalue_cast<BootstrapStatus::Recommendation>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_BootstrapStatus_Recommendation_toString(QScriptContext *context, QScriptEngine *engine)
{
    BootstrapStatus::Recommendation value = qscriptvalue_cast<BootstrapStatus::Recommendation>(context->thisObject());
    return QScriptValue(engine, qtscript_BootstrapStatus_Recommendation_toStringHelper(value));
}

static QScriptValue qtscript_create_BootstrapStatus_Recommendation_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_BootstrapStatus_Recommendation,
        qtscript_BootstrapStatus_Recommendation_valueOf, qtscript_BootstrapStatus_Recommendation_toString);
    qScriptRegisterMetaType<BootstrapStatus::Recommendation>(engine, qtscript_BootstrapStatus_Recommendation_toScriptValue,
        qtscript_BootstrapStatus_Recommendation_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 3; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_BootstrapStatus_Recommendation_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_BootstrapStatus_Recommendation_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// BootstrapStatus::Status
//

static const BootstrapStatus::Status qtscript_BootstrapStatus_Status_values[] = {
    BootstrapStatus::UnrecognizedStatus
    , BootstrapStatus::ConnectingToDirMirror
    , BootstrapStatus::HandshakingWithDirMirror
    , BootstrapStatus::CreatingOneHopCircuit
    , BootstrapStatus::RequestingNetworkStatus
    , BootstrapStatus::LoadingNetworkStatus
    , BootstrapStatus::LoadingAuthorityCertificates
    , BootstrapStatus::RequestingDescriptors
    , BootstrapStatus::LoadingDescriptors
    , BootstrapStatus::ConnectingToEntryGuard
    , BootstrapStatus::HandshakingWithEntryGuard
    , BootstrapStatus::EstablishingCircuit
    , BootstrapStatus::BootstrappingDone
};

static const char * const qtscript_BootstrapStatus_Status_keys[] = {
    "UnrecognizedStatus"
    , "ConnectingToDirMirror"
    , "HandshakingWithDirMirror"
    , "CreatingOneHopCircuit"
    , "RequestingNetworkStatus"
    , "LoadingNetworkStatus"
    , "LoadingAuthorityCertificates"
    , "RequestingDescriptors"
    , "LoadingDescriptors"
    , "ConnectingToEntryGuard"
    , "HandshakingWithEntryGuard"
    , "EstablishingCircuit"
    , "BootstrappingDone"
};

static QString qtscript_BootstrapStatus_Status_toStringHelper(BootstrapStatus::Status value)
{
    if ((value >= BootstrapStatus::UnrecognizedStatus) && (value <= BootstrapStatus::BootstrappingDone))
        return qtscript_BootstrapStatus_Status_keys[static_cast<int>(value)-static_cast<int>(BootstrapStatus::UnrecognizedStatus)];
    return QString();
}

static QScriptValue qtscript_BootstrapStatus_Status_toScriptValue(QScriptEngine *engine, const BootstrapStatus::Status &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("BootstrapStatus"));
    return clazz.property(qtscript_BootstrapStatus_Status_toStringHelper(value));
}

static void qtscript_BootstrapStatus_Status_fromScriptValue(const QScriptValue &value, BootstrapStatus::Status &out)
{
    out = qvariant_cast<BootstrapStatus::Status>(value.toVariant());
}

static QScriptValue qtscript_construct_BootstrapStatus_Status(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= BootstrapStatus::UnrecognizedStatus) && (arg <= BootstrapStatus::BootstrappingDone))
        return qScriptValueFromValue(engine,  static_cast<BootstrapStatus::Status>(arg));
    return context->throwError(QString::fromLatin1("Status(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_BootstrapStatus_Status_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    BootstrapStatus::Status value = qscriptvalue_cast<BootstrapStatus::Status>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_BootstrapStatus_Status_toString(QScriptContext *context, QScriptEngine *engine)
{
    BootstrapStatus::Status value = qscriptvalue_cast<BootstrapStatus::Status>(context->thisObject());
    return QScriptValue(engine, qtscript_BootstrapStatus_Status_toStringHelper(value));
}

static QScriptValue qtscript_create_BootstrapStatus_Status_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_BootstrapStatus_Status,
        qtscript_BootstrapStatus_Status_valueOf, qtscript_BootstrapStatus_Status_toString);
    qScriptRegisterMetaType<BootstrapStatus::Status>(engine, qtscript_BootstrapStatus_Status_toScriptValue,
        qtscript_BootstrapStatus_Status_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 13; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_BootstrapStatus_Status_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_BootstrapStatus_Status_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// BootstrapStatus
//

static QScriptValue qtscript_BootstrapStatus_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 6;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    BootstrapStatus* _q_self = qscriptvalue_cast<BootstrapStatus*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("BootstrapStatus.%0(): this object is not a BootstrapStatus")
            .arg(qtscript_BootstrapStatus_function_names[_id+3]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->description();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isValid();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        int _q_result = _q_self->percentComplete();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        BootstrapStatus::Recommendation _q_result = _q_self->recommendedAction();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        BootstrapStatus::Status _q_result = _q_self->status();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->warning();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6: {
    QString result = QString::fromLatin1("BootstrapStatus");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_BootstrapStatus_throw_ambiguity_error_helper(context,
        qtscript_BootstrapStatus_function_names[_id+3],
        qtscript_BootstrapStatus_function_signatures[_id+3]);
}

static QScriptValue qtscript_BootstrapStatus_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("BootstrapStatus(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        BootstrapStatus* _q_cpp_result = new BootstrapStatus();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        BootstrapStatus::Recommendation _q_result = BootstrapStatus::actionFromString(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        BootstrapStatus::Status _q_result = BootstrapStatus::statusFromString(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_BootstrapStatus_throw_ambiguity_error_helper(context,
        qtscript_BootstrapStatus_function_names[_id],
        qtscript_BootstrapStatus_function_signatures[_id]);
}

QScriptValue qtscript_create_BootstrapStatus_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<BootstrapStatus*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((BootstrapStatus*)0));
    for (int i = 0; i < 7; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_BootstrapStatus_prototype_call, qtscript_BootstrapStatus_function_lengths[i+3]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_BootstrapStatus_function_names[i+3]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<BootstrapStatus*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_BootstrapStatus_static_call, proto, qtscript_BootstrapStatus_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 2; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_BootstrapStatus_static_call,
            qtscript_BootstrapStatus_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_BootstrapStatus_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("Recommendation"),
        qtscript_create_BootstrapStatus_Recommendation_class(engine, ctor));
    ctor.setProperty(QString::fromLatin1("Status"),
        qtscript_create_BootstrapStatus_Status_class(engine, ctor));
    return ctor;
}
