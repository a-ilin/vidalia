#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <ControlReply.h>
#include <QVariant>
#include <ReplyLine.h>
#include <qlist.h>
#include <qstringlist.h>

static const char * const qtscript_ControlReply_function_names[] = {
    "ControlReply"
    // static
    // prototype
    , "appendLine"
    , "getData"
    , "getLine"
    , "getLines"
    , "getMessage"
    , "getStatus"
    , "toString"
};

static const char * const qtscript_ControlReply_function_signatures[] = {
    ""
    // static
    // prototype
    , "ReplyLine line"
    , ""
    , "int idx"
    , ""
    , ""
    , ""
    , ""
};

static const int qtscript_ControlReply_function_lengths[] = {
    0
    // static
    // prototype
    , 1
    , 0
    , 1
    , 0
    , 0
    , 0
    , 0
};

static QScriptValue qtscript_ControlReply_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("ControlReply::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(ControlReply*)
Q_DECLARE_METATYPE(ReplyLine)
Q_DECLARE_METATYPE(QList<ReplyLine>)

//
// ControlReply
//

static QScriptValue qtscript_ControlReply_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 7;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    ControlReply* _q_self = qscriptvalue_cast<ControlReply*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("ControlReply.%0(): this object is not a ControlReply")
            .arg(qtscript_ControlReply_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 1) {
        ReplyLine _q_arg0 = qscriptvalue_cast<ReplyLine>(context->argument(0));
        _q_self->appendLine(_q_arg0);
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
        ReplyLine _q_result = _q_self->getLine();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        int _q_arg0 = context->argument(0).toInt32();
        ReplyLine _q_result = _q_self->getLine(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QList<ReplyLine> _q_result = _q_self->getLines();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->getMessage();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->getStatus();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->toString();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlReply_throw_ambiguity_error_helper(context,
        qtscript_ControlReply_function_names[_id+1],
        qtscript_ControlReply_function_signatures[_id+1]);
}

static QScriptValue qtscript_ControlReply_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("ControlReply(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        ControlReply* _q_cpp_result = new ControlReply();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlReply_throw_ambiguity_error_helper(context,
        qtscript_ControlReply_function_names[_id],
        qtscript_ControlReply_function_signatures[_id]);
}

QScriptValue qtscript_create_ControlReply_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<ControlReply*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((ControlReply*)0));
    for (int i = 0; i < 7; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ControlReply_prototype_call, qtscript_ControlReply_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_ControlReply_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<ControlReply*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_ControlReply_static_call, proto, qtscript_ControlReply_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
