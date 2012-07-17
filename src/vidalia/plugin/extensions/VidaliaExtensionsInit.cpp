#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

const int classCount = 16;

QScriptValue qtscript_create_TorControl_class(QScriptEngine *engine);
QScriptValue qtscript_create_Circuit_class(QScriptEngine *engine);
QScriptValue qtscript_create_RouterDescriptor_class(QScriptEngine *engine);
QScriptValue qtscript_create_BootstrapStatus_class(QScriptEngine *engine);
QScriptValue qtscript_create_TorSignal_class(QScriptEngine *engine);
QScriptValue qtscript_create_ControlCommand_class(QScriptEngine *engine);
QScriptValue qtscript_create_ControlMethod_class(QScriptEngine *engine);
QScriptValue qtscript_create_ReplyLine_class(QScriptEngine *engine);
QScriptValue qtscript_create_Stream_class(QScriptEngine *engine);
QScriptValue qtscript_create_ControlReply_class(QScriptEngine *engine);
QScriptValue qtscript_create_RouterStatus_class(QScriptEngine *engine);
QScriptValue qtscript_create_ControlConnection_class(QScriptEngine *engine);
QScriptValue qtscript_create_TorEvents_class(QScriptEngine *engine);
QScriptValue qtscript_create_ControlSocket_class(QScriptEngine *engine);
QScriptValue qtscript_create_SendCommandEvent_class(QScriptEngine *engine);
QScriptValue qtscript_create_TorProcess_class(QScriptEngine *engine);

static const char * const vidalia_class_names[] = {
    "Circuit"
    , "RouterDescriptor"
    , "BootstrapStatus"
    , "TorSignal"
    , "ControlCommand"
    , "ControlMethod"
    , "ReplyLine"
    , "Stream"
    , "ControlReply"
    , "RouterStatus"
    , "ControlConnection"
    , "TorControl"
    , "TorEvents"
    , "ControlSocket"
    , "SendCommandEvent"
    , "TorProcess"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator vidalia_class_functions[] = {
    qtscript_create_Circuit_class
    , qtscript_create_RouterDescriptor_class
    , qtscript_create_BootstrapStatus_class
    , qtscript_create_TorSignal_class
    , qtscript_create_ControlCommand_class
    , qtscript_create_ControlMethod_class
    , qtscript_create_ReplyLine_class
    , qtscript_create_Stream_class
    , qtscript_create_ControlReply_class
    , qtscript_create_RouterStatus_class
    , qtscript_create_ControlConnection_class
    , qtscript_create_TorControl_class
    , qtscript_create_TorEvents_class
    , qtscript_create_ControlSocket_class
    , qtscript_create_SendCommandEvent_class
    , qtscript_create_TorProcess_class
};

void vidalia_initialize_bindings(QScriptValue &extensionObject)
{
    QScriptEngine *engine = extensionObject.engine();
    for (int i = 0; i < classCount; ++i) {
        extensionObject.setProperty(vidalia_class_names[i],
            vidalia_class_functions[i](engine),
            QScriptValue::SkipInEnumeration);
    }
}
