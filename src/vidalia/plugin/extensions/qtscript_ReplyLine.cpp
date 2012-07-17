#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <ReplyLine.h>
#include <QVariant>
#include <qstringlist.h>

static const char * const qtscript_ReplyLine_function_names[] = {
    "ReplyLine"
    // static
    // prototype
    , "appendData"
    , "getData"
    , "getMessage"
    , "getStatus"
    , "hasData"
    , "setMessage"
    , "setStatus"
    , "toString"
};

static const char * const qtscript_ReplyLine_function_signatures[] = {
    "\nString status, String message\nString status, String message, String data"
    // static
    // prototype
    , "String data"
    , ""
    , ""
    , ""
    , ""
    , "String msg"
    , "String status"
    , ""
};

static const int qtscript_ReplyLine_function_lengths[] = {
    3
    // static
    // prototype
    , 1
    , 0
    , 0
    , 0
    , 0
    , 1
    , 1
    , 0
};

static QScriptValue qtscript_ReplyLine_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("ReplyLine::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(ReplyLine*)

//
// ReplyLine
//

static QScriptValue qtscript_ReplyLine_prototype_call(QScriptContext *context, QScriptEngine *)
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
    ReplyLine* _q_self = qscriptvalue_cast<ReplyLine*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("ReplyLine.%0(): this object is not a ReplyLine")
            .arg(qtscript_ReplyLine_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->appendData(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QStringList _q_result = _q_self->getData();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->getMessage();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->getStatus();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->hasData();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->setMessage(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 6:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->setStatus(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 7:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->toString();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ReplyLine_throw_ambiguity_error_helper(context,
        qtscript_ReplyLine_function_names[_id+1],
        qtscript_ReplyLine_function_signatures[_id+1]);
}

static QScriptValue qtscript_ReplyLine_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("ReplyLine(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        ReplyLine* _q_cpp_result = new ReplyLine();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QString _q_arg1 = context->argument(1).toString();
        ReplyLine* _q_cpp_result = new ReplyLine(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 3) {
        QString _q_arg0 = context->argument(0).toString();
        QString _q_arg1 = context->argument(1).toString();
        QString _q_arg2 = context->argument(2).toString();
        ReplyLine* _q_cpp_result = new ReplyLine(_q_arg0, _q_arg1, _q_arg2);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ReplyLine_throw_ambiguity_error_helper(context,
        qtscript_ReplyLine_function_names[_id],
        qtscript_ReplyLine_function_signatures[_id]);
}

QScriptValue qtscript_create_ReplyLine_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<ReplyLine*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((ReplyLine*)0));
    for (int i = 0; i < 8; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ReplyLine_prototype_call, qtscript_ReplyLine_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_ReplyLine_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<ReplyLine*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_ReplyLine_static_call, proto, qtscript_ReplyLine_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
