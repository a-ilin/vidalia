#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <TorControl.h>
#include <QVariant>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qdatetime.h>
#include <qhostaddress.h>
#include <qlist.h>
#include <qobject.h>
#include <qstringlist.h>

static const char * const qtscript_TorControl_function_names[] = {
    "TorControl"
    // static
    // prototype
    , "authenticate"
    , "clearErrState"
    , "closeTorStdout"
    , "connect"
    , "disconnect"
    , "finished"
    , "getConf"
    , "getDescriptorAnnotations"
    , "getHiddenServiceConf"
    , "getInfo"
    , "getRouterDescriptorText"
    , "getSocksAddress"
    , "getSocksAddressList"
    , "getSocksPort"
    , "getSocksPortList"
    , "getTorVersion"
    , "getTorVersionString"
    , "ipToCountry"
    , "isAuthenticated"
    , "isCircuitEstablished"
    , "isConnected"
    , "isRunning"
    , "isVidaliaRunningTor"
    , "loadConf"
    , "resetConf"
    , "saveConf"
    , "setEvents"
    , "shouldContinue"
    , "start"
    , "stop"
    , "takeOwnership"
    , "useMicrodescriptors"
    , "toString"
};

static const char * const qtscript_TorControl_function_signatures[] = {
    ""
    // static
    // prototype
    , "QByteArray cookie, String errmsg\nString password, String errmsg"
    , ""
    , ""
    , "QHostAddress address, unsigned short port\nString path"
    , ""
    , "int exitCode, ExitStatus exitStatus"
    , "HashMap map, String errmsg\nString key, List value, String errmsg\nString key, String errmsg\nList keys, String errmsg"
    , "String id, String errmsg"
    , "String key, String errmsg"
    , "String key, String errmsg\nList keys, String errmsg"
    , "String id, String errmsg"
    , "String errmsg"
    , "String errmsg"
    , "String errmsg"
    , "String errmsg"
    , ""
    , ""
    , "QHostAddress ip, String errmsg"
    , ""
    , ""
    , ""
    , ""
    , ""
    , "String contents, String errmsg"
    , "String key, String errmsg\nList keys, String errmsg"
    , "String errmsg"
    , "String errmsg"
    , "String errmsg"
    , "String tor, List args"
    , "String errmsg"
    , "String errmsg"
    , "String errmsg"
""
};

static const int qtscript_TorControl_function_lengths[] = {
    0
    // static
    // prototype
    , 2
    , 0
    , 0
    , 2
    , 0
    , 2
    , 3
    , 2
    , 2
    , 2
    , 2
    , 1
    , 1
    , 1
    , 1
    , 0
    , 0
    , 2
    , 0
    , 0
    , 0
    , 0
    , 0
    , 2
    , 2
    , 1
    , 1
    , 1
    , 2
    , 1
    , 1
    , 1
    , 0
};

static QScriptValue qtscript_TorControl_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("TorControl::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(TorControl*)
Q_DECLARE_METATYPE(QString*)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(int*)
Q_DECLARE_METATYPE(QProcess::ExitStatus*)
template <> \
struct QMetaTypeId< QHash<QString,QStringList> > \
{ \
    enum { Defined = 1 }; \
    static int qt_metatype_id() \
    { \
        static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); \
        if (!metatype_id) \
            metatype_id = qRegisterMetaType< QHash<QString,QStringList> >("QHash<QString,QStringList>"); \
        return metatype_id; \
    } \
};
template <> \
struct QMetaTypeId< QHash<QString,QString> > \
{ \
    enum { Defined = 1 }; \
    static int qt_metatype_id() \
    { \
        static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); \
        if (!metatype_id) \
            metatype_id = qRegisterMetaType< QHash<QString,QString> >("QHash<QString,QString>"); \
        return metatype_id; \
    } \
};
Q_DECLARE_METATYPE(QList<unsigned short>)

//
// TorControl
//

static QScriptValue qtscript_TorControl_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 32;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    TorControl* _q_self = qscriptvalue_cast<TorControl*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("TorControl.%0(): this object is not a TorControl")
            .arg(qtscript_TorControl_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->authenticate();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        if ((qMetaTypeId<QByteArray>() == context->argument(0).toVariant().userType())) {
            QByteArray _q_arg0 = qscriptvalue_cast<QByteArray>(context->argument(0));
            bool _q_result = _q_self->authenticate(_q_arg0);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isString()) {
            QString _q_arg0 = context->argument(0).toString();
            bool _q_result = _q_self->authenticate(_q_arg0);
            return QScriptValue(context->engine(), _q_result);
        }
    }
    if (context->argumentCount() == 2) {
        if ((qMetaTypeId<QByteArray>() == context->argument(0).toVariant().userType())
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QByteArray _q_arg0 = qscriptvalue_cast<QByteArray>(context->argument(0));
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            bool _q_result = _q_self->authenticate(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isString()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QString _q_arg0 = context->argument(0).toString();
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            bool _q_result = _q_self->authenticate(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        }
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        _q_self->clearErrState();
        return context->engine()->undefinedValue();
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        _q_self->closeTorStdout();
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
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

    case 4:
    if (context->argumentCount() == 0) {
        _q_self->disconnect();
        return context->engine()->undefinedValue();
    }
    break;

    case 5:
    if (context->argumentCount() == 2) {
        int* _q_arg0 = qscriptvalue_cast<int*>(context->argument(0));
        QProcess::ExitStatus* _q_arg1 = qscriptvalue_cast<QProcess::ExitStatus*>(context->argument(1));
        bool _q_result = _q_self->finished(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 1) {
        if ((qMetaTypeId<QHash<QString,QStringList> >() == context->argument(0).toVariant().userType())) {
            QHash<QString,QStringList> _q_arg0 = qscriptvalue_cast<QHash<QString,QStringList> >(context->argument(0));
            bool _q_result = _q_self->getConf(_q_arg0);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isString()) {
            QString _q_arg0 = context->argument(0).toString();
            QVariant _q_result = _q_self->getConf(_q_arg0);
            return qScriptValueFromValue(context->engine(), _q_result);
        } else if (context->argument(0).isArray()) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            QMap<QString,QVariant> _q_result = _q_self->getConf(_q_arg0);
            return qScriptValueFromValue(context->engine(), _q_result);
        }
    }
    if (context->argumentCount() == 2) {
        if ((qMetaTypeId<QHash<QString,QStringList> >() == context->argument(0).toVariant().userType())
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QHash<QString,QStringList> _q_arg0 = qscriptvalue_cast<QHash<QString,QStringList> >(context->argument(0));
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            bool _q_result = _q_self->getConf(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isString()
            && context->argument(1).isArray()) {
            QString _q_arg0 = context->argument(0).toString();
            QStringList _q_arg1;
            qScriptValueToSequence(context->argument(1), _q_arg1);
            bool _q_result = _q_self->getConf(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isString()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QString _q_arg0 = context->argument(0).toString();
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            QVariant _q_result = _q_self->getConf(_q_arg0, _q_arg1);
            return qScriptValueFromValue(context->engine(), _q_result);
        } else if (context->argument(0).isArray()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            QMap<QString,QVariant> _q_result = _q_self->getConf(_q_arg0, _q_arg1);
            return qScriptValueFromValue(context->engine(), _q_result);
        }
    }
    if (context->argumentCount() == 3) {
        QString _q_arg0 = context->argument(0).toString();
        QStringList _q_arg1;
        qScriptValueToSequence(context->argument(1), _q_arg1);
        QString* _q_arg2 = qscriptvalue_cast<QString*>(context->argument(2));
        bool _q_result = _q_self->getConf(_q_arg0, _q_arg1, _q_arg2);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 7:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        QHash<QString,QString> _q_result = _q_self->getDescriptorAnnotations(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        QHash<QString,QString> _q_result = _q_self->getDescriptorAnnotations(_q_arg0, _q_arg1);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 8:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        QString _q_result = _q_self->getHiddenServiceConf(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        QString _q_result = _q_self->getHiddenServiceConf(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 9:
    if (context->argumentCount() == 1) {
        if (context->argument(0).isString()) {
            QString _q_arg0 = context->argument(0).toString();
            QVariant _q_result = _q_self->getInfo(_q_arg0);
            return qScriptValueFromValue(context->engine(), _q_result);
        } else if (context->argument(0).isArray()) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            QMap<QString,QVariant> _q_result = _q_self->getInfo(_q_arg0);
            return qScriptValueFromValue(context->engine(), _q_result);
        }
    }
    if (context->argumentCount() == 2) {
        if (context->argument(0).isString()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QString _q_arg0 = context->argument(0).toString();
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            QVariant _q_result = _q_self->getInfo(_q_arg0, _q_arg1);
            return qScriptValueFromValue(context->engine(), _q_result);
        } else if (context->argument(0).isArray()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            QMap<QString,QVariant> _q_result = _q_self->getInfo(_q_arg0, _q_arg1);
            return qScriptValueFromValue(context->engine(), _q_result);
        }
    }
    break;

    case 10:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        QStringList _q_result = _q_self->getRouterDescriptorText(_q_arg0);
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        QStringList _q_result = _q_self->getRouterDescriptorText(_q_arg0, _q_arg1);
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 11:
    if (context->argumentCount() == 0) {
        QHostAddress _q_result = _q_self->getSocksAddress();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        QHostAddress _q_result = _q_self->getSocksAddress(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 12:
    if (context->argumentCount() == 0) {
        QStringList _q_result = _q_self->getSocksAddressList();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        QStringList _q_result = _q_self->getSocksAddressList(_q_arg0);
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 13:
    if (context->argumentCount() == 0) {
        unsigned short _q_result = _q_self->getSocksPort();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        unsigned short _q_result = _q_self->getSocksPort(_q_arg0);
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 14:
    if (context->argumentCount() == 0) {
        QList<unsigned short> _q_result = _q_self->getSocksPortList();
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        QList<unsigned short> _q_result = _q_self->getSocksPortList(_q_arg0);
        return qScriptValueFromSequence(context->engine(), _q_result);
    }
    break;

    case 15:
    if (context->argumentCount() == 0) {
        uint _q_result = _q_self->getTorVersion();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 16:
    if (context->argumentCount() == 0) {
        QString _q_result = _q_self->getTorVersionString();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 17:
    if (context->argumentCount() == 1) {
        QHostAddress _q_arg0 = qscriptvalue_cast<QHostAddress>(context->argument(0));
        QString _q_result = _q_self->ipToCountry(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        QHostAddress _q_arg0 = qscriptvalue_cast<QHostAddress>(context->argument(0));
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        QString _q_result = _q_self->ipToCountry(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 18:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isAuthenticated();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 19:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isCircuitEstablished();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 20:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isConnected();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 21:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isRunning();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 22:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isVidaliaRunningTor();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 23:
    if (context->argumentCount() == 1) {
        QString _q_arg0 = context->argument(0).toString();
        bool _q_result = _q_self->loadConf(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
        bool _q_result = _q_self->loadConf(_q_arg0, _q_arg1);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 24:
    if (context->argumentCount() == 1) {
        if (context->argument(0).isString()) {
            QString _q_arg0 = context->argument(0).toString();
            bool _q_result = _q_self->resetConf(_q_arg0);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isArray()) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            bool _q_result = _q_self->resetConf(_q_arg0);
            return QScriptValue(context->engine(), _q_result);
        }
    }
    if (context->argumentCount() == 2) {
        if (context->argument(0).isString()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QString _q_arg0 = context->argument(0).toString();
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            bool _q_result = _q_self->resetConf(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        } else if (context->argument(0).isArray()
            && qscriptvalue_cast<QString*>(context->argument(1))) {
            QStringList _q_arg0;
            qScriptValueToSequence(context->argument(0), _q_arg0);
            QString* _q_arg1 = qscriptvalue_cast<QString*>(context->argument(1));
            bool _q_result = _q_self->resetConf(_q_arg0, _q_arg1);
            return QScriptValue(context->engine(), _q_result);
        }
    }
    break;

    case 25:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->saveConf();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->saveConf(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 26:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->setEvents();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->setEvents(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 27:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->shouldContinue();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->shouldContinue(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 28:
    if (context->argumentCount() == 2) {
        QString _q_arg0 = context->argument(0).toString();
        QStringList _q_arg1;
        qScriptValueToSequence(context->argument(1), _q_arg1);
        _q_self->start(_q_arg0, _q_arg1);
        return context->engine()->undefinedValue();
    }
    break;

    case 29:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->stop();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->stop(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 30:
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->takeOwnership(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 31:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->useMicrodescriptors();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QString* _q_arg0 = qscriptvalue_cast<QString*>(context->argument(0));
        bool _q_result = _q_self->useMicrodescriptors(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 32: {
    QString result = QString::fromLatin1("TorControl");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorControl_throw_ambiguity_error_helper(context,
        qtscript_TorControl_function_names[_id+1],
        qtscript_TorControl_function_signatures[_id+1]);
}

static QScriptValue qtscript_TorControl_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    return context->throwError(QScriptContext::TypeError,
        QString::fromLatin1("TorControl cannot be constructed"));
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_TorControl_throw_ambiguity_error_helper(context,
        qtscript_TorControl_function_names[_id],
        qtscript_TorControl_function_signatures[_id]);
}

static QScriptValue qtscript_TorControl_toScriptValue(QScriptEngine *engine, TorControl* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_TorControl_fromScriptValue(const QScriptValue &value, TorControl* &out)
{
    out = qobject_cast<TorControl*>(value.toQObject());
}

QScriptValue qtscript_create_TorControl_class(QScriptEngine *engine)
{
    engine->setDefaultPrototype(qMetaTypeId<TorControl*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((TorControl*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QObject*>()));
    for (int i = 0; i < 33; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_TorControl_prototype_call, qtscript_TorControl_function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_TorControl_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<TorControl*>(engine, qtscript_TorControl_toScriptValue,
        qtscript_TorControl_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_TorControl_static_call, proto, qtscript_TorControl_function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
