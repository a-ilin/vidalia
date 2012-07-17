#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <ControlCommand.h>
#include <QVariant>
#include <qstringlist.h>

static const char * const qtscript_ControlCommand_function_names[] = {
    "ControlCommand"
    // static
    // prototype
    , "addArgument"
    , "addArguments"
    , "appendData"
    , "keyword"
    , "setKeyword"
    , "toString"
};

static const char * const qtscript_ControlCommand_function_signatures[] = {
    "\nString keyword\nString keyword, String arg\nString keyword, List args"
    // static
    // prototype
    , "String arg"
    , "List args"
    , "String data"
    , ""
    , "String keyword"
    , ""
};

static const int qtscript_ControlCommand_function_lengths[] = {
    2
    // static
    // prototype
    , 1
    , 1
    , 1
    , 0
    , 1
    , 0
};

static QScriptValue qtscript_ControlCommand_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("ControlCommand::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(ControlCommand*)

//
// ControlCommand
//

static QScriptValue qtscript_ControlCommand_prototype_call(QScriptContext *context, QScriptEngine *)
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
    ControlCommand* _q_self = qscriptvalue_cast<ControlCommand*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("ControlCommand.%0(): this object is not a ControlCommand")
            .arg(qtscript_ControlCommand_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->addArgument(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 1:
    if (context->argumentCount() == 1) {
        QStringList _q_arg0;
        qScriptValueToSequence(context->argument(0), _q_arg0);
        _q_self->addArguments(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->appendData(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->keyword();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->setKeyword(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->toString();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlCommand_throw_ambiguity_error_helper(context,
        qtscript_ControlCommand_function_names[_id+1],
        qtscript_ControlCommand_function_signatures[_id+1]);
}

static QScriptValue qtscript_ControlCommand_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("ControlCommand(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 0) {
        ControlCommand* _q_cpp_result = new ControlCommand();
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        ControlCommand* _q_cpp_result = new ControlCommand(_q_arg0);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
        return _q_result;
    } else if (context->argumentCount() == 2) {
        if (context->argument(0).isString()
            && context->argument(1).isString()) {
            QString _q_arg0 = context->argument(0).toString();
            QString _q_arg1 = context->argument(1).toString();
            ControlCommand* _q_cpp_result = new ControlCommand(_q_arg0, _q_arg1);
            QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
            return _q_result;
        } else if (context->argument(0).isString()
            && context->argument(1).isArray()) {
            QString _q_arg0 = context->argument(0).toString();
            QStringList _q_arg1;
            qScriptValueToSequence(context->argument(1), _q_arg1);
            ControlCommand* _q_cpp_result = new ControlCommand(_q_arg0, _q_arg1);
            QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue(_q_cpp_result));
            return _q_result;
        }
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_ControlCommand_throw_ambiguity_error_helper(context,
        qtscript_ControlCommand_function_names[_id],
        qtscript_ControlCommand_function_signatures[_id]);
}

QScriptValue qtscript_create_ControlCommand_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<ControlCommand*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((ControlCommand*)0));
    for (int i = 0; i < 6; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_ControlCommand_prototype_call, qtscript_ControlCommand_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_ControlCommand_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<ControlCommand*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_ControlCommand_static_call, proto, qtscript_ControlCommand_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
