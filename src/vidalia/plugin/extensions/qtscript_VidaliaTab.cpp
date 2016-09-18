#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <VidaliaTab.h>
#include <QVariant>
#include <qaction.h>
#include <qbitmap.h>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qcursor.h>
#include <qevent.h>
#include <qfont.h>
#include <qgraphicseffect.h>
#include <qgraphicsproxywidget.h>
#include <qicon.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <qinputcontext.h>
#endif

#include <qkeysequence.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlocale.h>
#include <qmargins.h>
#include <qobject.h>
#include <qpaintdevice.h>
#include <qpaintengine.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstyle.h>
#include <qwidget.h>

#include "qtscriptshell_VidaliaTab.h"

static const char * const qtscript_VidaliaTab_function_names[] = {
    "VidaliaTab"
    // static
    // prototype
    , "getSetting"
    , "getTitle"
    , "saveSetting"
    , "setOnTop"
    , "setShortcut"
    , "setTitle"
    , "toString"
};

static const char * const qtscript_VidaliaTab_function_signatures[] = {
    "String title, String name, QWidget parent"
    // static
    // prototype
    , "String name, Object defaultValue"
    , ""
    , "String name, Object value"
    , "bool top"
    , "String shortcut, char slot"
    , "String title"
""
};

static const int qtscript_VidaliaTab_function_lengths[] = {
    3
    // static
    // prototype
    , 2
    , 0
    , 2
    , 1
    , 2
    , 1
    , 0
};

static QScriptValue qtscript_VidaliaTab_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("VidaliaTab::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QtScriptShell_VidaliaTab*)
Q_DECLARE_METATYPE(char*)

//
// VidaliaTab
//

static QScriptValue qtscript_VidaliaTab_prototype_call(QScriptContext *context, QScriptEngine *)
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
    VidaliaTab* _q_self = qscriptvalue_cast<VidaliaTab*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("VidaliaTab.%0(): this object is not a VidaliaTab")
            .arg(qtscript_VidaliaTab_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QVariant _q_arg1 = context->argument(1).toVariant();
        QVariant _q_result = _q_self->getSetting(_q_arg0, _q_arg1);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->getTitle();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QVariant _q_arg1 = context->argument(1).toVariant();
        _q_self->saveSetting(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 1) {
        bool _q_arg0 = context->argument(0).toBoolean();
        _q_self->setOnTop(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 4:
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        char* _q_arg1 = qscriptvalue_cast<char*>(context->argument(1));
        _q_self->setShortcut(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 5:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        _q_self->setTitle(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 6: {
    QString result = QString::fromLatin1("VidaliaTab");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_VidaliaTab_throw_ambiguity_error_helper(context,
        qtscript_VidaliaTab_function_names[_id+1],
        qtscript_VidaliaTab_function_signatures[_id+1]);
}

static QScriptValue qtscript_VidaliaTab_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("VidaliaTab(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QString _q_arg1 = context->argument(1).toString();
        QtScriptShell_VidaliaTab* _q_cpp_result = new QtScriptShell_VidaliaTab(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (VidaliaTab*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 3) {
        QString _q_arg0 = context->argument(0).toString();
        QString _q_arg1 = context->argument(1).toString();
        QWidget* _q_arg2 = qscriptvalue_cast<QWidget*>(context->argument(2));
        QtScriptShell_VidaliaTab* _q_cpp_result = new QtScriptShell_VidaliaTab(_q_arg0, _q_arg1, _q_arg2);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (VidaliaTab*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_VidaliaTab_throw_ambiguity_error_helper(context,
        qtscript_VidaliaTab_function_names[_id],
        qtscript_VidaliaTab_function_signatures[_id]);
}

static QScriptValue qtscript_VidaliaTab_toScriptValue(QScriptEngine *engine, VidaliaTab* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_VidaliaTab_fromScriptValue(const QScriptValue &value, VidaliaTab* &out)
{
    out = qobject_cast<VidaliaTab*>(value.toQObject());
}

QScriptValue qtscript_create_VidaliaTab_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<VidaliaTab*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((VidaliaTab*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QWidget*>()));
    for (int i = 0; i < 7; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_VidaliaTab_prototype_call, qtscript_VidaliaTab_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_VidaliaTab_function_names[i+1]),
            fun);
    }

    qScriptRegisterMetaType<VidaliaTab*>(engine, qtscript_VidaliaTab_toScriptValue,
        qtscript_VidaliaTab_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_VidaliaTab_static_call, proto, qtscript_VidaliaTab_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
