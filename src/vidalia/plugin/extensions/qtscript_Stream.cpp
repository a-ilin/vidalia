#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <Stream.h>
#include <QVariant>
#include <Stream.h>

static const char * const qtscript_Stream_function_names[] = {
    "Stream"
    // static
    , "fromString"
    , "isValidStreamId"
    , "toStatus"
    , "tr"
    , "trUtf8"
    // prototype
    , "circuitId"
    , "id"
    , "isValid"
    , "status"
    , "statusString"
    , "target"
    , "targetAddress"
    , "targetPort"
    , "toString"
};

static const char * const qtscript_Stream_function_signatures[] = {
    "\nString streamId, Status status, String circuitId, String address, unsigned short port\nString streamId, Status status, String circuitId, String target"
    // static
    , "String stream"
    , "String streamId"
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

static const int qtscript_Stream_function_lengths[] = {
    5
    // static
    , 1
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

static QScriptValue qtscript_Stream_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("Stream::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(Stream*)
Q_DECLARE_METATYPE(Stream::Status)
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
// Stream::Status
//

static const Stream::Status qtscript_Stream_Status_values[] = {
    Stream::Unknown
    , Stream::New
    , Stream::NewResolve
    , Stream::SentConnect
    , Stream::SentResolve
    , Stream::Succeeded
    , Stream::Failed
    , Stream::Closed
    , Stream::Detached
    , Stream::Remap
};

static const char * const qtscript_Stream_Status_keys[] = {
    "Unknown"
    , "New"
    , "NewResolve"
    , "SentConnect"
    , "SentResolve"
    , "Succeeded"
    , "Failed"
    , "Closed"
    , "Detached"
    , "Remap"
};

static QString qtscript_Stream_Status_toStringHelper(Stream::Status value)
{
    if ((value >= Stream::Unknown) && (value <= Stream::Remap))
        return qtscript_Stream_Status_keys[static_cast<int>(value)-static_cast<int>(Stream::Unknown)];
    return QString();
}

static QScriptValue qtscript_Stream_Status_toScriptValue(QScriptEngine *engine, const Stream::Status &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("Stream"));
    return clazz.property(qtscript_Stream_Status_toStringHelper(value));
}

static void qtscript_Stream_Status_fromScriptValue(const QScriptValue &value, Stream::Status &out)
{
    out = qvariant_cast<Stream::Status>(value.toVariant());
}

static QScriptValue qtscript_construct_Stream_Status(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= Stream::Unknown) && (arg <= Stream::Remap))
        return qScriptValueFromValue(engine,  static_cast<Stream::Status>(arg));
    return context->throwError(QString::fromLatin1("Status(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_Stream_Status_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    Stream::Status value = qscriptvalue_cast<Stream::Status>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_Stream_Status_toString(QScriptContext *context, QScriptEngine *engine)
{
    Stream::Status value = qscriptvalue_cast<Stream::Status>(context->thisObject());
    return QScriptValue(engine, qtscript_Stream_Status_toStringHelper(value));
}

static QScriptValue qtscript_create_Stream_Status_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_Stream_Status,
        qtscript_Stream_Status_valueOf, qtscript_Stream_Status_toString);
    qScriptRegisterMetaType<Stream::Status>(engine, qtscript_Stream_Status_toScriptValue,
        qtscript_Stream_Status_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 10; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_Stream_Status_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_Stream_Status_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// Stream
//

static QScriptValue qtscript_Stream_prototype_call(QScriptContext *context, QScriptEngine *)
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
    Stream* _q_self = qscriptvalue_cast<Stream*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("Stream.%0(): this object is not a Stream")
            .arg(qtscript_Stream_function_names[_id+6]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->circuitId();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->id();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isValid();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        Stream::Status _q_result = _q_self->status();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->statusString();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->target();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->targetAddress();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 0) {
        unsigned short _q_result = _q_self->targetPort();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 8: {
    QString result = QString::fromLatin1("Stream");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_Stream_throw_ambiguity_error_helper(context,
        qtscript_Stream_function_names[_id+6],
        qtscript_Stream_function_signatures[_id+6]);
}

static QScriptValue qtscript_Stream_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("Stream(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        Stream* _q_cpp_result = new Stream();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 4) {
        QString _q_arg0 = context->argument(0).toString();
        Stream::Status _q_arg1 = qscriptvalue_cast<Stream::Status>(context->argument(1));
        QString _q_arg2 = context->argument(2).toString();
        QString _q_arg3 = context->argument(3).toString();
        Stream* _q_cpp_result = new Stream(_q_arg0, _q_arg1, _q_arg2, _q_arg3);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 5) {
        QString _q_arg0 = context->argument(0).toString();
        Stream::Status _q_arg1 = qscriptvalue_cast<Stream::Status>(context->argument(1));
        QString _q_arg2 = context->argument(2).toString();
        QString _q_arg3 = context->argument(3).toString();
        unsigned short _q_arg4 = qscriptvalue_cast<unsigned short>(context->argument(4));
        Stream* _q_cpp_result = new Stream(_q_arg0, _q_arg1, _q_arg2, _q_arg3, _q_arg4);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        Stream _q_result = Stream::fromString(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        bool _q_result = Stream::isValidStreamId(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        Stream::Status _q_result = Stream::toStatus(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 1) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        QString _q_result = Stream::tr(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        QString _q_result = Stream::tr(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 3) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        int _q_arg2 = context->argument(2).toInt32();
        QString _q_result = Stream::tr(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 1) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        QString _q_result = Stream::trUtf8(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        QString _q_result = Stream::trUtf8(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 3) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        int _q_arg2 = context->argument(2).toInt32();
        QString _q_result = Stream::trUtf8(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_Stream_throw_ambiguity_error_helper(context,
        qtscript_Stream_function_names[_id],
        qtscript_Stream_function_signatures[_id]);
}

QScriptValue qtscript_create_Stream_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<Stream*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((Stream*)0));
    for (int i = 0; i < 9; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_Stream_prototype_call, qtscript_Stream_function_lengths[i+6]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_Stream_function_names[i+6]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<Stream*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_Stream_static_call, proto, qtscript_Stream_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 5; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_Stream_static_call,
            qtscript_Stream_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_Stream_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("Status"),
        qtscript_create_Stream_Status_class(engine, ctor));
    return ctor;
}
