#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <RouterDescriptor.h>
#include <QVariant>
#include <RouterDescriptor.h>
#include <RouterStatus.h>
#include <TorControl.h>
#include <qdatetime.h>
#include <qhostaddress.h>
#include <qstringlist.h>

static const char * const qtscript_RouterDescriptor_function_names[] = {
    "RouterDescriptor"
    // static
    , "fromTorControl"
    , "tr"
    , "trUtf8"
    // prototype
    , "appendRouterStatusInfo"
    , "averageBandwidth"
    , "burstBandwidth"
    , "contact"
    , "dirPort"
    , "exitPolicy"
    , "family"
    , "fingerprint"
    , "hibernating"
    , "id"
    , "ip"
    , "isEmpty"
    , "location"
    , "name"
    , "observedBandwidth"
    , "offline"
    , "onionKey"
    , "online"
    , "orPort"
    , "platform"
    , "published"
    , "setLocation"
    , "setOffline"
    , "status"
    , "uptime"
    , "toString"
};

static const char * const qtscript_RouterDescriptor_function_signatures[] = {
    "List descriptor, bool microdesc\nbool microdesc"
    // static
    , "TorControl tc"
    , "char sourceText, char disambiguation\nchar sourceText, char disambiguation, int n"
    , "char sourceText, char disambiguation\nchar sourceText, char disambiguation, int n"
    // prototype
    , "RouterStatus rs"
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
    , "String location"
    , "bool offline"
    , ""
    , ""
""
};

static const int qtscript_RouterDescriptor_function_lengths[] = {
    2
    // static
    , 1
    , 3
    , 3
    // prototype
    , 1
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
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 1
    , 1
    , 0
    , 0
    , 0
};

static QScriptValue qtscript_RouterDescriptor_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("RouterDescriptor::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(RouterDescriptor*)
Q_DECLARE_METATYPE(RouterDescriptor::RouterStatusEnum)
Q_DECLARE_METATYPE(RouterStatus)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(TorControl*)
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
// RouterDescriptor::RouterStatusEnum
//

static const RouterDescriptor::RouterStatusEnum qtscript_RouterDescriptor_RouterStatusEnum_values[] = {
    RouterDescriptor::Online
    , RouterDescriptor::Hibernating
    , RouterDescriptor::Offline
};

static const char * const qtscript_RouterDescriptor_RouterStatusEnum_keys[] = {
    "Online"
    , "Hibernating"
    , "Offline"
};

static QString qtscript_RouterDescriptor_RouterStatusEnum_toStringHelper(RouterDescriptor::RouterStatusEnum value)
{
    if ((value >= RouterDescriptor::Online) && (value <= RouterDescriptor::Offline))
        return qtscript_RouterDescriptor_RouterStatusEnum_keys[static_cast<int>(value)-static_cast<int>(RouterDescriptor::Online)];
    return QString();
}

static QScriptValue qtscript_RouterDescriptor_RouterStatusEnum_toScriptValue(QScriptEngine *engine, const RouterDescriptor::RouterStatusEnum &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("RouterDescriptor"));
    return clazz.property(qtscript_RouterDescriptor_RouterStatusEnum_toStringHelper(value));
}

static void qtscript_RouterDescriptor_RouterStatusEnum_fromScriptValue(const QScriptValue &value, RouterDescriptor::RouterStatusEnum &out)
{
    out = qvariant_cast<RouterDescriptor::RouterStatusEnum>(value.toVariant());
}

static QScriptValue qtscript_construct_RouterDescriptor_RouterStatusEnum(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= RouterDescriptor::Online) && (arg <= RouterDescriptor::Offline))
        return qScriptValueFromValue(engine,  static_cast<RouterDescriptor::RouterStatusEnum>(arg));
    return context->throwError(QString::fromLatin1("RouterStatusEnum(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_RouterDescriptor_RouterStatusEnum_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    RouterDescriptor::RouterStatusEnum value = qscriptvalue_cast<RouterDescriptor::RouterStatusEnum>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_RouterDescriptor_RouterStatusEnum_toString(QScriptContext *context, QScriptEngine *engine)
{
    RouterDescriptor::RouterStatusEnum value = qscriptvalue_cast<RouterDescriptor::RouterStatusEnum>(context->thisObject());
    return QScriptValue(engine, qtscript_RouterDescriptor_RouterStatusEnum_toStringHelper(value));
}

static QScriptValue qtscript_create_RouterDescriptor_RouterStatusEnum_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_RouterDescriptor_RouterStatusEnum,
        qtscript_RouterDescriptor_RouterStatusEnum_valueOf, qtscript_RouterDescriptor_RouterStatusEnum_toString);
    qScriptRegisterMetaType<RouterDescriptor::RouterStatusEnum>(engine, qtscript_RouterDescriptor_RouterStatusEnum_toScriptValue,
        qtscript_RouterDescriptor_RouterStatusEnum_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 3; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_RouterDescriptor_RouterStatusEnum_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_RouterDescriptor_RouterStatusEnum_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// RouterDescriptor
//

static QScriptValue qtscript_RouterDescriptor_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 25;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    RouterDescriptor* _q_self = qscriptvalue_cast<RouterDescriptor*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("RouterDescriptor.%0(): this object is not a RouterDescriptor")
            .arg(qtscript_RouterDescriptor_function_names[_id+4]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 1) {
        RouterStatus _q_arg0 = qscriptvalue_cast<RouterStatus>(context->argument(0));
        _q_self->appendRouterStatusInfo(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        unsigned long long _q_result = _q_self->averageBandwidth();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        unsigned long long _q_result = _q_self->burstBandwidth();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->contact();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        unsigned short _q_result = _q_self->dirPort();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->exitPolicy();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->family();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->fingerprint();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 8:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->hibernating();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 9:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->id();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 10:
    if (context->argumentCount() == 0) {
        QHostAddress _q_result = _q_self->ip();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 11:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isEmpty();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 12:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->location();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 13:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->name();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 14:
    if (context->argumentCount() == 0) {
        unsigned long long _q_result = _q_self->observedBandwidth();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 15:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->offline();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 16:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->onionKey();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 17:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->online();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 18:
    if (context->argumentCount() == 0) {
        unsigned short _q_result = _q_self->orPort();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 19:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->platform();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 20:
    if (context->argumentCount() == 0) {
        QDateTime _q_result = _q_self->published();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 21:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->setLocation(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 22:
    if (context->argumentCount() == 1) {
        bool _q_arg0 = context->argument(0).toBoolean();
        _q_self->setOffline(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 23:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->status();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 24:
    if (context->argumentCount() == 0) {
        unsigned long long _q_result = _q_self->uptime();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 25: {
    QString result = QString::fromLatin1("RouterDescriptor");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_RouterDescriptor_throw_ambiguity_error_helper(context,
        qtscript_RouterDescriptor_function_names[_id+4],
        qtscript_RouterDescriptor_function_signatures[_id+4]);
}

static QScriptValue qtscript_RouterDescriptor_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("RouterDescriptor(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        RouterDescriptor* _q_cpp_result = new RouterDescriptor();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 1) {
        if (context->argument(0).isArray()) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            RouterDescriptor* _q_cpp_result = new RouterDescriptor(_q_arg0);
            QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
            return _q_result;
        } else if (context->argument(0).isBoolean()) {
            bool _q_arg0 = context->argument(0).toBoolean();
            RouterDescriptor* _q_cpp_result = new RouterDescriptor(_q_arg0);
            QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
            return _q_result;
        }
    } else if (context->argumentCount() == 2) {
        QStringList _q_arg0;
        qScriptValueToSequence(context->argument(0), _q_arg0);
        bool _q_arg1 = context->argument(1).toBoolean();
        RouterDescriptor* _q_cpp_result = new RouterDescriptor(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        TorControl* _q_arg0 = qscriptvalue_cast<TorControl*>(context->argument(0));
        RouterDescriptor* _q_result = RouterDescriptor::fromTorControl(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        QString _q_result = RouterDescriptor::tr(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        QString _q_result = RouterDescriptor::tr(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 3) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        int _q_arg2 = context->argument(2).toInt32();
        QString _q_result = RouterDescriptor::tr(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 1) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        QString _q_result = RouterDescriptor::trUtf8(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        QString _q_result = RouterDescriptor::trUtf8(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 3) {
        char* _q_arg0 = qscriptvalue_cast<char*>(context->argument(0));
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        int _q_arg2 = context->argument(2).toInt32();
        QString _q_result = RouterDescriptor::trUtf8(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_RouterDescriptor_throw_ambiguity_error_helper(context,
        qtscript_RouterDescriptor_function_names[_id],
        qtscript_RouterDescriptor_function_signatures[_id]);
}

QScriptValue qtscript_create_RouterDescriptor_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<RouterDescriptor*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((RouterDescriptor*)0));
    for (int i = 0; i < 26; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_RouterDescriptor_prototype_call, qtscript_RouterDescriptor_function_lengths[i+4]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_RouterDescriptor_function_names[i+4]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<RouterDescriptor*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_RouterDescriptor_static_call, proto, qtscript_RouterDescriptor_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 3; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_RouterDescriptor_static_call,
            qtscript_RouterDescriptor_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_RouterDescriptor_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("RouterStatusEnum"),
        qtscript_create_RouterDescriptor_RouterStatusEnum_class(engine, ctor));
    return ctor;
}
