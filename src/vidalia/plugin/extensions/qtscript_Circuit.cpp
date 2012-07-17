#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <Circuit.h>
#include <QVariant>
#include <qstringlist.h>

static const char * const qtscript_Circuit_function_names[] = {
    "Circuit"
    // static
    , "isValidCircuitId"
    , "toStatus"
    , "tr"
    , "trUtf8"
    // prototype
    , "id"
    , "isValid"
    , "length"
    , "purpose"
    , "routerIDs"
    , "routerNames"
    , "status"
    , "statusString"
    , "toString"
};

static const char * const qtscript_Circuit_function_signatures[] = {
    "\nString circuit"
    // static
    , "String circId"
    , "String strStatus"
    , "char sourceText, char disambiguation\nchar sourceText, char disambiguation, int n"
    , "char sourceText, char disambiguation\nchar sourceText, char disambiguation, int n"
    // prototype
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

static const int qtscript_Circuit_function_lengths[] = {
    1
    // static
    , 1
    , 1
    , 3
    , 3
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
};

static QScriptValue qtscript_Circuit_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("Circuit::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(Circuit*)
Q_DECLARE_METATYPE(Circuit::Purpose)
Q_DECLARE_METATYPE(Circuit::BuildFlag)
Q_DECLARE_METATYPE(Circuit::Status)
Q_DECLARE_METATYPE(char*)

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
// Circuit::Purpose
//

static const Circuit::Purpose qtscript_Circuit_Purpose_values[] = {
    Circuit::General
    , Circuit::HsClientIntro
    , Circuit::HsClientRend
    , Circuit::HsServiceIntro
    , Circuit::HsServiceRend
    , Circuit::Testing
    , Circuit::Controller
};

static const char * const qtscript_Circuit_Purpose_keys[] = {
    "General"
    , "HsClientIntro"
    , "HsClientRend"
    , "HsServiceIntro"
    , "HsServiceRend"
    , "Testing"
    , "Controller"
};

static QString qtscript_Circuit_Purpose_toStringHelper(Circuit::Purpose value)
{
    if ((value >= Circuit::General) && (value <= Circuit::Controller))
        return qtscript_Circuit_Purpose_keys[static_cast<int>(value)-static_cast<int>(Circuit::General)];
    return QString();
}

static QScriptValue qtscript_Circuit_Purpose_toScriptValue(QScriptEngine *engine, const Circuit::Purpose &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("Circuit"));
    return clazz.property(qtscript_Circuit_Purpose_toStringHelper(value));
}

static void qtscript_Circuit_Purpose_fromScriptValue(const QScriptValue &value, Circuit::Purpose &out)
{
    out = qvariant_cast<Circuit::Purpose>(value.toVariant());
}

static QScriptValue qtscript_construct_Circuit_Purpose(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= Circuit::General) && (arg <= Circuit::Controller))
        return qScriptValueFromValue(engine,  static_cast<Circuit::Purpose>(arg));
    return context->throwError(QString::fromLatin1("Purpose(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_Circuit_Purpose_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    Circuit::Purpose value = qscriptvalue_cast<Circuit::Purpose>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_Circuit_Purpose_toString(QScriptContext *context, QScriptEngine *engine)
{
    Circuit::Purpose value = qscriptvalue_cast<Circuit::Purpose>(context->thisObject());
    return QScriptValue(engine, qtscript_Circuit_Purpose_toStringHelper(value));
}

static QScriptValue qtscript_create_Circuit_Purpose_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_Circuit_Purpose,
        qtscript_Circuit_Purpose_valueOf, qtscript_Circuit_Purpose_toString);
    qScriptRegisterMetaType<Circuit::Purpose>(engine, qtscript_Circuit_Purpose_toScriptValue,
        qtscript_Circuit_Purpose_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 7; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_Circuit_Purpose_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_Circuit_Purpose_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// Circuit::BuildFlag
//

static const Circuit::BuildFlag qtscript_Circuit_BuildFlag_values[] = {
    Circuit::None
    , Circuit::OneHopTunnel
    , Circuit::IsInternal
    , Circuit::NeedCapacity
    , Circuit::NeedUptime
};

static const char * const qtscript_Circuit_BuildFlag_keys[] = {
    "None"
    , "OneHopTunnel"
    , "IsInternal"
    , "NeedCapacity"
    , "NeedUptime"
};

static QString qtscript_Circuit_BuildFlag_toStringHelper(Circuit::BuildFlag value)
{
    for (int i = 0; i < 5; ++i) {
        if (qtscript_Circuit_BuildFlag_values[i] == value)
            return QString::fromLatin1(qtscript_Circuit_BuildFlag_keys[i]);
    }
    return QString();
}

static QScriptValue qtscript_Circuit_BuildFlag_toScriptValue(QScriptEngine *engine, const Circuit::BuildFlag &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("Circuit"));
    return clazz.property(qtscript_Circuit_BuildFlag_toStringHelper(value));
}

static void qtscript_Circuit_BuildFlag_fromScriptValue(const QScriptValue &value, Circuit::BuildFlag &out)
{
    out = qvariant_cast<Circuit::BuildFlag>(value.toVariant());
}

static QScriptValue qtscript_construct_Circuit_BuildFlag(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    for (int i = 0; i < 5; ++i) {
        if (qtscript_Circuit_BuildFlag_values[i] == arg)
            return qScriptValueFromValue(engine,  static_cast<Circuit::BuildFlag>(arg));
    }
    return context->throwError(QString::fromLatin1("BuildFlag(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_Circuit_BuildFlag_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    Circuit::BuildFlag value = qscriptvalue_cast<Circuit::BuildFlag>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_Circuit_BuildFlag_toString(QScriptContext *context, QScriptEngine *engine)
{
    Circuit::BuildFlag value = qscriptvalue_cast<Circuit::BuildFlag>(context->thisObject());
    return QScriptValue(engine, qtscript_Circuit_BuildFlag_toStringHelper(value));
}

static QScriptValue qtscript_create_Circuit_BuildFlag_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_Circuit_BuildFlag,
        qtscript_Circuit_BuildFlag_valueOf, qtscript_Circuit_BuildFlag_toString);
    qScriptRegisterMetaType<Circuit::BuildFlag>(engine, qtscript_Circuit_BuildFlag_toScriptValue,
        qtscript_Circuit_BuildFlag_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 5; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_Circuit_BuildFlag_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_Circuit_BuildFlag_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// Circuit::Status
//

static const Circuit::Status qtscript_Circuit_Status_values[] = {
    Circuit::Unknown
    , Circuit::Launched
    , Circuit::Built
    , Circuit::Extended
    , Circuit::Failed
    , Circuit::Closed
};

static const char * const qtscript_Circuit_Status_keys[] = {
    "Unknown"
    , "Launched"
    , "Built"
    , "Extended"
    , "Failed"
    , "Closed"
};

static QString qtscript_Circuit_Status_toStringHelper(Circuit::Status value)
{
    if ((value >= Circuit::Unknown) && (value <= Circuit::Closed))
        return qtscript_Circuit_Status_keys[static_cast<int>(value)-static_cast<int>(Circuit::Unknown)];
    return QString();
}

static QScriptValue qtscript_Circuit_Status_toScriptValue(QScriptEngine *engine, const Circuit::Status &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("Circuit"));
    return clazz.property(qtscript_Circuit_Status_toStringHelper(value));
}

static void qtscript_Circuit_Status_fromScriptValue(const QScriptValue &value, Circuit::Status &out)
{
    out = qvariant_cast<Circuit::Status>(value.toVariant());
}

static QScriptValue qtscript_construct_Circuit_Status(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= Circuit::Unknown) && (arg <= Circuit::Closed))
        return qScriptValueFromValue(engine,  static_cast<Circuit::Status>(arg));
    return context->throwError(QString::fromLatin1("Status(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_Circuit_Status_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    Circuit::Status value = qscriptvalue_cast<Circuit::Status>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_Circuit_Status_toString(QScriptContext *context, QScriptEngine *engine)
{
    Circuit::Status value = qscriptvalue_cast<Circuit::Status>(context->thisObject());
    return QScriptValue(engine, qtscript_Circuit_Status_toStringHelper(value));
}

static QScriptValue qtscript_create_Circuit_Status_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_Circuit_Status,
        qtscript_Circuit_Status_valueOf, qtscript_Circuit_Status_toString);
    qScriptRegisterMetaType<Circuit::Status>(engine, qtscript_Circuit_Status_toScriptValue,
        qtscript_Circuit_Status_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 6; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_Circuit_Status_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_Circuit_Status_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// Circuit
//

static QScriptValue qtscript_Circuit_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 8;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    Circuit* _q_self = qscriptvalue_cast<Circuit*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("Circuit.%0(): this object is not a Circuit")
            .arg(qtscript_Circuit_function_names[_id+5]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->id();
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
        uint _q_result = _q_self->length();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        Circuit::Purpose _q_result = _q_self->purpose();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        QStringList _q_result = _q_self->routerIDs();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QStringList _q_result = _q_self->routerNames();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        Circuit::Status _q_result = _q_self->status();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->statusString();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 8: {
    QString result = QString::fromLatin1("Circuit");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_Circuit_throw_ambiguity_error_helper(context,
        qtscript_Circuit_function_names[_id+5],
        qtscript_Circuit_function_signatures[_id+5]);
}

static QScriptValue qtscript_Circuit_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("Circuit(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        Circuit* _q_cpp_result = new Circuit();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        Circuit* _q_cpp_result = new Circuit(_q_arg0);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        bool _q_result = Circuit::isValidCircuitId(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        Circuit::Status _q_result = Circuit::toStatus(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 1) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        QString _q_result = Circuit::tr(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        QString _q_result = Circuit::tr(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 3) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        int _q_arg2 = context->argument(2).toInt32();
        QString _q_result = Circuit::tr(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 1) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        QString _q_result = Circuit::trUtf8(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        QString _q_result = Circuit::trUtf8(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 3) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        int _q_arg2 = context->argument(2).toInt32();
        QString _q_result = Circuit::trUtf8(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_Circuit_throw_ambiguity_error_helper(context,
        qtscript_Circuit_function_names[_id],
        qtscript_Circuit_function_signatures[_id]);
}

QScriptValue qtscript_create_Circuit_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<Circuit*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((Circuit*)0));
    for (int i = 0; i < 9; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_Circuit_prototype_call, qtscript_Circuit_function_lengths[i+5]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_Circuit_function_names[i+5]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<Circuit*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_Circuit_static_call, proto, qtscript_Circuit_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 4; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_Circuit_static_call,
            qtscript_Circuit_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_Circuit_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("Purpose"),
        qtscript_create_Circuit_Purpose_class(engine, ctor));
    ctor.setProperty(QString::fromLatin1("BuildFlag"),
        qtscript_create_Circuit_BuildFlag_class(engine, ctor));
    ctor.setProperty(QString::fromLatin1("Status"),
        qtscript_create_Circuit_Status_class(engine, ctor));
    return ctor;
}
