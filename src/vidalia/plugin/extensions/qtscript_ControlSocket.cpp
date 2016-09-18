#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <ControlSocket.h>
#include <ControlCommand.h>
#include <ControlReply.h>
#include <QVariant>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qhostaddress.h>
#include <qlist.h>
#include <qobject.h>

#include "qtscriptshell_ControlSocket.h"

static const char * const qtscript_ControlSocket_function_names[] = {
    "ControlSocket"
    // static
    , "toString"
    // prototype
    , "canReadLine"
    , "connectToHost"
    , "connectToServer"
    , "disconnectFromHost"
    , "disconnectFromServer"
    , "getMethod"
    , "isConnected"
    , "readReply"
    , "sendCommand"
};

static const char * const qtscript_ControlSocket_function_signatures[] = {
    "Method method"
    // static
    , "SocketError error"
    // prototype
    , ""
    , "QHostAddress address, unsigned short port"
    , "String name"
    , ""
    , ""
    , ""
    , ""
    , "ControlReply reply, String errmsg"
    , "ControlCommand cmd, String errmsg"
};

static const int qtscript_ControlSocket_function_lengths[] = {
    1
    // static
    , 1
    // prototype
    , 0
    , 2
    , 1
    , 0
    , 0
    , 0
    , 0
    , 2
    , 2
};

static QScriptValue qtscript_ControlSocket_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("ControlSocket::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(ControlSocket*)
Q_DECLARE_METATYPE(QtScriptShell_ControlSocket*)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(ControlMethod::Method)
Q_DECLARE_METATYPE(ControlReply)
Q_DECLARE_METATYPE(QString*)
Q_DECLARE_METATYPE(ControlCommand)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
#endif

//
// ControlSocket
//

static QScriptValue qtscript_ControlSocket_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 9;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    ControlSocket* _q_self = qscriptvalue_cast<ControlSocket*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("ControlSocket.%0(): this object is not a ControlSocket")
            .arg(qtscript_ControlSocket_function_names[_id+2]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->canReadLine();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 2) {
        QHostAddress _q_arg0 = qscriptvalue_cast<QHostAddress>(context->argument(0));
        unsigned short _q_arg1 = qscriptvalue_cast<unsigned short>(context->argument(1));
        _q_self->connectToHost(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->connectToServer(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        _q_self->disconnectFromHost();
        return context->engine()->undefinedValue();
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        _q_self->disconnectFromServer();
        return context->engine()->undefinedValue();
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        ControlMethod::Method _q_result = _q_self->getMethod();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isConnected();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 1) {
        ControlReply _q_arg0 = qscriptvalue_cast<ControlReply>(context->argument(0));
        bool _q_result = _q_self->readReply(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        ControlReply _q_arg0 = qscriptvalue_cast<ControlReply>(context->argument(0));
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        bool _q_result = _q_self->readReply(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 8:
    if (context->argumentCount() == 1) {
        ControlCommand _q_arg0 = qscriptvalue_cast<ControlCommand>(context->argument(0));
        bool _q_result = _q_self->sendCommand(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        ControlCommand _q_arg0 = qscriptvalue_cast<ControlCommand>(context->argument(0));
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        bool _q_result = _q_self->sendCommand(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlSocket_throw_ambiguity_error_helper(context,
        qtscript_ControlSocket_function_names[_id+2],
        qtscript_ControlSocket_function_signatures[_id+2]);
}

static QScriptValue qtscript_ControlSocket_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("ControlSocket(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        QtScriptShell_ControlSocket* _q_cpp_result = new QtScriptShell_ControlSocket();
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (ControlSocket*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 1) {
        ControlMethod::Method _q_arg0 = qscriptvalue_cast<ControlMethod::Method>(context->argument(0));
        QtScriptShell_ControlSocket* _q_cpp_result = new QtScriptShell_ControlSocket(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (ControlSocket*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QAbstractSocket::SocketError _q_arg0 = qscriptvalue_cast<QAbstractSocket::SocketError>(context->argument(0));
        QString _q_result = ControlSocket::toString(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlSocket_throw_ambiguity_error_helper(context,
        qtscript_ControlSocket_function_names[_id],
        qtscript_ControlSocket_function_signatures[_id]);
}

static QScriptValue qtscript_ControlSocket_toScriptValue(QScriptEngine *engine, ControlSocket* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_ControlSocket_fromScriptValue(const QScriptValue &value, ControlSocket* &out)
{
    out = qobject_cast<ControlSocket*>(value.toQObject());
}

QScriptValue qtscript_create_ControlSocket_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<ControlSocket*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((ControlSocket*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QObject*>()));
    for (int i = 0; i < 9; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ControlSocket_prototype_call, qtscript_ControlSocket_function_lengths[i+2]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_ControlSocket_function_names[i+2]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<ControlSocket*>(engine, qtscript_ControlSocket_toScriptValue, 
        qtscript_ControlSocket_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_ControlSocket_static_call, proto, qtscript_ControlSocket_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 1; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ControlSocket_static_call,
            qtscript_ControlSocket_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_ControlSocket_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    return ctor;
}
