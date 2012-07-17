#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <ControlConnection.h>
#include <ControlCommand.h>
#include <ControlReply.h>
#include <QVariant>
#include <TorEvents.h>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qhostaddress.h>
#include <qlist.h>
#include <qobject.h>

#include "qtscriptshell_ControlConnection.h"

static const char * const qtscript_ControlConnection_function_names[] = {
    "ControlConnection"
    // static
    // prototype
    , "cancelConnect"
    , "connect"
    , "disconnect"
    , "isConnected"
    , "send"
    , "status"
    , "toString"
};

static const char * const qtscript_ControlConnection_function_signatures[] = {
    "Method method, TorEvents events"
    // static
    // prototype
    , ""
    , "QHostAddress addr, unsigned short port\nString addr"
    , ""
    , ""
    , "ControlCommand cmd, ControlReply reply, String errmsg\nControlCommand cmd, String errmsg"
    , ""
""
};

static const int qtscript_ControlConnection_function_lengths[] = {
    2
    // static
    // prototype
    , 0
    , 2
    , 0
    , 0
    , 3
    , 0
    , 0
};

static QScriptValue qtscript_ControlConnection_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("ControlConnection::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(ControlConnection*)
Q_DECLARE_METATYPE(QtScriptShell_ControlConnection*)
Q_DECLARE_METATYPE(ControlConnection::Status)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(ControlCommand)
Q_DECLARE_METATYPE(ControlReply)
Q_DECLARE_METATYPE(QString*)
Q_DECLARE_METATYPE(ControlMethod::Method)
Q_DECLARE_METATYPE(TorEvents*)

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
// ControlConnection::Status
//

static const ControlConnection::Status qtscript_ControlConnection_Status_values[] = {
    ControlConnection::Unset
    , ControlConnection::Disconnected
    , ControlConnection::Disconnecting
    , ControlConnection::Connecting
    , ControlConnection::Connected
};

static const char * const qtscript_ControlConnection_Status_keys[] = {
    "Unset"
    , "Disconnected"
    , "Disconnecting"
    , "Connecting"
    , "Connected"
};

static QString qtscript_ControlConnection_Status_toStringHelper(ControlConnection::Status value)
{
    if ((value >= ControlConnection::Unset) && (value <= ControlConnection::Connected))
        return qtscript_ControlConnection_Status_keys[static_cast<int>(value)-static_cast<int>(ControlConnection::Unset)];
    return QString();
}

static QScriptValue qtscript_ControlConnection_Status_toScriptValue(QScriptEngine *engine, const ControlConnection::Status &value)
{
    QScriptValue clazz = engine->globalObject().property(QString::fromLatin1("ControlConnection"));
    return clazz.property(qtscript_ControlConnection_Status_toStringHelper(value));
}

static void qtscript_ControlConnection_Status_fromScriptValue(const QScriptValue &value, ControlConnection::Status &out)
{
    out = qvariant_cast<ControlConnection::Status>(value.toVariant());
}

static QScriptValue qtscript_construct_ControlConnection_Status(QScriptContext *context, QScriptEngine *engine)
{
    int arg = context->argument(0).toInt32();
    if ((arg >= ControlConnection::Unset) && (arg <= ControlConnection::Connected))
        return qScriptValueFromValue(engine,  static_cast<ControlConnection::Status>(arg));
    return context->throwError(QString::fromLatin1("Status(): invalid enum value (%0)").arg(arg));
}

static QScriptValue qtscript_ControlConnection_Status_valueOf(QScriptContext *context, QScriptEngine *engine)
{
    ControlConnection::Status value = qscriptvalue_cast<ControlConnection::Status>(context->thisObject());
    return QScriptValue(engine, static_cast<int>(value));
}

static QScriptValue qtscript_ControlConnection_Status_toString(QScriptContext *context, QScriptEngine *engine)
{
    ControlConnection::Status value = qscriptvalue_cast<ControlConnection::Status>(context->thisObject());
    return QScriptValue(engine, qtscript_ControlConnection_Status_toStringHelper(value));
}

static QScriptValue qtscript_create_ControlConnection_Status_class(QScriptEngine *engine, QScriptValue &clazz)
{
    QScriptValue ctor = qtscript_create_enum_class_helper(
        engine, qtscript_construct_ControlConnection_Status,
        qtscript_ControlConnection_Status_valueOf, qtscript_ControlConnection_Status_toString);
    qScriptRegisterMetaType<ControlConnection::Status>(engine, qtscript_ControlConnection_Status_toScriptValue,
        qtscript_ControlConnection_Status_fromScriptValue, ctor.property(QString::fromLatin1("prototype")));
    for (int i = 0; i < 5; ++i) {
        clazz.setProperty(QString::fromLatin1(qtscript_ControlConnection_Status_keys[i]),
            engine->newVariant(qVariantFromValue(qtscript_ControlConnection_Status_values[i])),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    return ctor;
}

//
// ControlConnection
//

static QScriptValue qtscript_ControlConnection_prototype_call(QScriptContext *context, QScriptEngine *)
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
    ControlConnection* _q_self = qscriptvalue_cast<ControlConnection*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("ControlConnection.%0(): this object is not a ControlConnection")
            .arg(qtscript_ControlConnection_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        _q_self->cancelConnect();
        return context->engine()->undefinedValue();
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->connect(_q_arg0);
        return context->engine()->undefinedValue();
    }
    if (context->argumentCount() == 2) {
        QHostAddress _q_arg0 = qscriptvalue_cast<QHostAddress>(context->argument(0));
        unsigned short _q_arg1 = qscriptvalue_cast<unsigned short>(context->argument(1));
        _q_self->connect(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        _q_self->disconnect();
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isConnected();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 1) {
        ControlCommand _q_arg0 = qscriptvalue_cast<ControlCommand>(context->argument(0));
        bool _q_result = _q_self->send(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        if ((qMetaTypeId<ControlCommand>() == context->argument(0).toVariant().userType())
            && (qMetaTypeId<ControlReply>() == context->argument(1).toVariant().userType())) {
            ControlCommand _q_arg0 = qscriptvalue_cast<ControlCommand>(context->argument(0));
            ControlReply _q_arg1 = qscriptvalue_cast<ControlReply>(context->argument(1));
            bool _q_result = _q_self->send(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        } else if ((qMetaTypeId<ControlCommand>() == context->argument(0).toVariant().userType())
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            ControlCommand _q_arg0 = qscriptvalue_cast<ControlCommand>(context->argument(0));
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            bool _q_result = _q_self->send(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        }
    }
    if (context->argumentCount() == 3) {
        ControlCommand _q_arg0 = qscriptvalue_cast<ControlCommand>(context->argument(0));
        ControlReply _q_arg1 = qscriptvalue_cast<ControlReply>(context->argument(1));
        QString* _q_arg2 = qscriptvalue_cast<QString*>(context->argument(2));
        bool _q_result = _q_self->send(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        ControlConnection::Status _q_result = _q_self->status();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 6: {
    QString result = QString::fromLatin1("ControlConnection");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlConnection_throw_ambiguity_error_helper(context,
        qtscript_ControlConnection_function_names[_id+1],
        qtscript_ControlConnection_function_signatures[_id+1]);
}

static QScriptValue qtscript_ControlConnection_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("ControlConnection(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 1) {
        ControlMethod::Method _q_arg0 = qscriptvalue_cast<ControlMethod::Method>(context->argument(0));
        QtScriptShell_ControlConnection* _q_cpp_result = new QtScriptShell_ControlConnection(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (ControlConnection*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 2) {
        ControlMethod::Method _q_arg0 = qscriptvalue_cast<ControlMethod::Method>(context->argument(0));
        TorEvents* _q_arg1 = qscriptvalue_cast<TorEvents*>(context->argument(1));
        QtScriptShell_ControlConnection* _q_cpp_result = new QtScriptShell_ControlConnection(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (ControlConnection*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlConnection_throw_ambiguity_error_helper(context,
        qtscript_ControlConnection_function_names[_id],
        qtscript_ControlConnection_function_signatures[_id]);
}

static QScriptValue qtscript_ControlConnection_toScriptValue(QScriptEngine *engine, ControlConnection* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_ControlConnection_fromScriptValue(const QScriptValue &value, ControlConnection* &out)
{
    out = qobject_cast<ControlConnection*>(value.toQObject());
}

QScriptValue qtscript_create_ControlConnection_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<ControlConnection*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((ControlConnection*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QObject*>()));
    for (int i = 0; i < 7; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ControlConnection_prototype_call, qtscript_ControlConnection_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_ControlConnection_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<ControlConnection*>(engine, qtscript_ControlConnection_toScriptValue, 
        qtscript_ControlConnection_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_ControlConnection_static_call, proto, qtscript_ControlConnection_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    ctor.setProperty(QString::fromLatin1("Status"),
        qtscript_create_ControlConnection_Status_class(engine, ctor));
    return ctor;
}
