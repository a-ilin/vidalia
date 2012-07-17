#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <TorEvents.h>
#include <BootstrapStatus.h>
#include <Circuit.h>
#include <ControlReply.h>
#include <QVariant>
#include <ReplyLine.h>
#include <Stream.h>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qdatetime.h>
#include <qhostaddress.h>
#include <qlist.h>
#include <qobject.h>
#include <qpair.h>
#include <qstringlist.h>

#include "qtscriptshell_TorEvents.h"

static const char * const qtscript_TorEvents_function_names[] = {
    "TorEvents"
    // static
    , "toString"
    // prototype
    , "handleEvent"
};

static const char * const qtscript_TorEvents_function_signatures[] = {
    "QObject parent"
    // static
    , "Event e"
    // prototype
    , "ControlReply reply"
};

static const int qtscript_TorEvents_function_lengths[] = {
    1
    // static
    , 1
    // prototype
    , 1
};

static QScriptValue qtscript_TorEvents_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("TorEvents::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(TorEvents*)
Q_DECLARE_METATYPE(QtScriptShell_TorEvents*)
Q_DECLARE_METATYPE(TorEvents::Event)
Q_DECLARE_METATYPE(ControlReply)

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
// TorEvents::Event
//

static const TorEvents::Event qtscript_TorEvents_Event_values[] = {
    TorEvents::Unknown
    , TorEvents::Bandwidth
    , TorEvents::LogDebug
    , TorEvents::LogInfo
    , TorEvents::LogNotice
    , TorEvents::LogWarn
    , TorEvents::LogError
    , TorEvents::CircuitStatus
    , TorEvents::StreamStatus
    , TorEvents::OrConnStatus
    , TorEvents::NewDescriptor
    , TorEvents::AddressMap
    , TorEvents::GeneralStatus
    , TorEvents::ClientStatus
    , TorEvents::ServerStatus
    , TorEvents::NewConsensus
    , TorEvents::StreamBandwidth
};

static const char * const qtscript_TorEvents_Event_keys[] = {
    "Unknown"
    , "Bandwidth"
    , "LogDebug"
    , "LogInfo"
    , "LogNotice"
    , "LogWarn"
    , "LogError"
    , "CircuitStatus"
    , "StreamStatus"
    , "OrConnStatus"
    , "NewDescriptor"
    , "AddressMap"
    , "GeneralStatus"
    , "ClientStatus"
    , "ServerStatus"
    , "NewConsensus"
    , "StreamBandwidth"
};

static QString qtscript_TorEvents_Event_toStringHelper(TorEvents::Event value)
{
    if ((value >= TorEvents::Unknown) && (value <= TorEvents::StreamBandwidth))
        return qtscript_TorEvents_Event_keys[static_cast<int>(value)-static_cast<int>(TorEvents::Unknown)];
    return QString();
}

static QScriptValue qtscript_TorEvents_Event_toScriptValue(QScriptEngine *engine, const TorEvents::Event &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("TorEvents"));
    return clazz.property(qtscript_TorEvents_Event_toStringHelper(value));
}

static void qtscript_TorEvents_Event_fromScriptValue(const QScriptValue &value, TorEvents::Event &out)
{
    out = qvariant_cast<TorEvents::Event>(value.toVariant());
}

static QScriptValue qtscript_construct_TorEvents_Event(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= TorEvents::Unknown) && (arg <= TorEvents::StreamBandwidth))
        return qScriptValueFromValue(engine,  static_cast<TorEvents::Event>(arg));
    return context->throwError(QString::fromLatin1("Event(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_TorEvents_Event_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    TorEvents::Event value = qscriptvalue_cast<TorEvents::Event>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_TorEvents_Event_toString(QScriptContext *context, QScriptEngine *engine)
{
    TorEvents::Event value = qscriptvalue_cast<TorEvents::Event>(context->thisObject());
    return QScriptValue(engine, qtscript_TorEvents_Event_toStringHelper(value));
}

static QScriptValue qtscript_create_TorEvents_Event_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_TorEvents_Event,
        qtscript_TorEvents_Event_valueOf, qtscript_TorEvents_Event_toString);
    qScriptRegisterMetaType<TorEvents::Event>(engine, qtscript_TorEvents_Event_toScriptValue,
        qtscript_TorEvents_Event_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 17; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_TorEvents_Event_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_TorEvents_Event_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// TorEvents
//

static QScriptValue qtscript_TorEvents_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 1;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    TorEvents* _q_self = qscriptvalue_cast<TorEvents*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("TorEvents.%0(): this object is not a TorEvents")
            .arg(qtscript_TorEvents_function_names[_id+2]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 1) {
        ControlReply _q_arg0 = qscriptvalue_cast<ControlReply>(context->argument(0));
        _q_self->handleEvent(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorEvents_throw_ambiguity_error_helper(context,
        qtscript_TorEvents_function_names[_id+2],
        qtscript_TorEvents_function_signatures[_id+2]);
}

static QScriptValue qtscript_TorEvents_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("TorEvents(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_TorEvents* _q_cpp_result = new QtScriptShell_TorEvents();
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (TorEvents*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QObject* _q_arg0 = context->argument(0).toQObject();
        QtScriptShell_TorEvents* _q_cpp_result = new QtScriptShell_TorEvents(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (TorEvents*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        TorEvents::Event _q_arg0 = qscriptvalue_cast<TorEvents::Event>(context->argument(0));
        QString _q_result = TorEvents::toString(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorEvents_throw_ambiguity_error_helper(context,
        qtscript_TorEvents_function_names[_id],
        qtscript_TorEvents_function_signatures[_id]);
}

static QScriptValue qtscript_TorEvents_toScriptValue(QScriptEngine *engine, TorEvents* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_TorEvents_fromScriptValue(const QScriptValue &value, TorEvents* &out)
{
    out = qobject_cast<TorEvents*>(value.toQObject());
}

QScriptValue qtscript_create_TorEvents_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<TorEvents*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((TorEvents*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QObject*>()));
    for (int i = 0; i < 1; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_TorEvents_prototype_call, qtscript_TorEvents_function_lengths[i+2]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_TorEvents_function_names[i+2]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<TorEvents*>(engine, qtscript_TorEvents_toScriptValue, 
        qtscript_TorEvents_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_TorEvents_static_call, proto, qtscript_TorEvents_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 1; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_TorEvents_static_call,
            qtscript_TorEvents_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_TorEvents_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    ctor.setProperty(QString::fromLatin1("Event"),
        qtscript_create_TorEvents_Event_class(engine, ctor));
    return ctor;
}
