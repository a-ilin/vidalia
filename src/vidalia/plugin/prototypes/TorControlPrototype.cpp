#include "TorControlPrototype.h"

#define GET_AND_CALL(type, func, res) \
  type obj = qscriptvalue_cast<type>(thisObject()); \
  if(obj) \
    res = obj->func;

#define MERGE2(result, errmsg) \
  QVariant(QList<QVariant>() << result << errmsg);

#define DEF_TYPE0(type, retType, func, call) \
retType \
type##Prototype::func \
{ \
  type *obj = qscriptvalue_cast<type *>(thisObject()); \
  if(obj) \
    return obj->call; \
}

#define DEF_TYPE1(type, resType, func, call) \
QVariant \
type##Prototype::func \
{ \
  resType res; \
  QString errmsg; \
\
  type *obj = qscriptvalue_cast<type *>(thisObject()); \
  if(obj) \
    res = obj->call; \
  QList<QVariant> vals; \
  vals << res << QVariant(errmsg); \
\
  return vals; \
}

#define DEF_TYPE2(type, resType, ansType, func, call) \
QVariant \
type##Prototype::func \
{ \
  resType res; \
  ansType ans; \
  QString errmsg; \
\
  type *obj = qscriptvalue_cast<type *>(thisObject()); \
  if(obj) \
    res = obj->call; \
  QList<QVariant> vals; \
  vals << QVariant(ans) << res << QVariant(errmsg); \
\
  return vals; \
}

TorControlPrototype::TorControlPrototype()
  : QObject(), QScriptable() {}

int 
TorControlPrototype::metaTypeId() {
  return qMetaTypeId<TorControl *>();
}

QString
TorControlPrototype::name() {
  return QString("TorControl");
}

DEF_TYPE0(TorControl, void, 
    start(const QString &tor, const QStringList &args),
    start(tor, args))

DEF_TYPE1(TorControl, bool,
    stop(),
    stop(&errmsg))

DEF_TYPE0(TorControl, bool,
    isRunning(),
    isRunning())

DEF_TYPE0(TorControl, bool,
    isVidaliaRunningTor(),
    isVidaliaRunningTor())

DEF_TYPE0(TorControl, void,
    closeTorStdout(),
    closeTorStdout())

DEF_TYPE0(TorControl, void,
    connect(const QHostAddress &address, quint16 port),
    connect(address, port))

DEF_TYPE0(TorControl, void,
    connect(const QString &path),
    connect(path))

DEF_TYPE0(TorControl, void,
    disconnect(),
    disconnect())

DEF_TYPE0(TorControl, bool,
    isConnected(),
    isConnected())

DEF_TYPE1(TorControl, bool,
    authenticate(const QByteArray cookie),
    authenticate(cookie, &errmsg))

DEF_TYPE1(TorControl, bool,
    authenticate(const QString &password),
    authenticate(password, &errmsg))

// TODO: make a QVariant for this two
//QVariant
//TorControlPrototype::protocolInfo()
//{
//  ProtocolInfo info;
//  QString errmsg;

//  GET_AND_CALL(TorControl *, protocolInfo(&errmsg), info)

//  return MERGE2(info, errmsg);
//}

//BootstrapStatus 
//TorControlPrototype::bootstrapStatus(QString *errmsg)
//{
//  BootstrapStatus status;
//  QString errmsg;

//  GET_AND_CALL(TorControl *, protocolInfo(&errmsg), status)

//  return MERGE2(status, errmsg);
//}

DEF_TYPE0(TorControl, bool,
    isCircuitEstablished(),
    isCircuitEstablished())

DEF_TYPE1(TorControl, bool,
  getInfo(QHash<QString,QString> &map),
  getInfo(map, &errmsg))

// TODO: this one may be useless
//QVariant
//TorControlPrototype::getInfo(QString key)
//{
//  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());
//  QString val, *errmsg = new QString();
//  bool res = false;
//  QList<QVariant> vals;

//  if(obj)
//    res = obj->getInfo(key, val, errmsg);

//  vals.append(QVariant(res));
//  vals.append(QVariant(val));
//  vals.append(QVariant(*errmsg));

//  return QVariant(vals);
//}

// TODO: There is no StringList, this may be useless
//DEF_TYPE1(TorControl, QVariantMap,
//  getInfo(const QStringList &keys),
//  getInfo(keys, &errmsg))

DEF_TYPE1(TorControl, QVariant,
    getInfo(const QString &key),
    getInfo(key, &errmsg))

DEF_TYPE1(TorControl, bool,
  signal(TorSignal::Signal sig),
  signal(sig, &errmsg))

// TODO: QVariant don't like QHostAddress
//DEF_TYPE1(TorControl, QHostAddress,
//    getSocksAddress(),
//    getSocksAddress(&errmsg))

// TODO: make it a QVariant(QList<QVariant>() << QVariant(QString) <<
// QVariant(QString) ...
QStringList 
TorControlPrototype::getSocksAddressList(QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getSocksAddressList(errmsg);
}

DEF_TYPE1(TorControl, quint16,
    getSocksPort(),
    getSocksPort(&errmsg))

// TODO: same as getSocksAddressList but with quint16
QList<quint16> 
TorControlPrototype::getSocksPortList(QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getSocksPortList(errmsg);
}

DEF_TYPE0(TorControl, QString,
    getTorVersionString(),
    getTorVersionString())

DEF_TYPE0(TorControl, quint32,
    getTorVersion(),
    getTorVersion())

DEF_TYPE1(TorControl, bool,
    setEvent(TorEvents::Event e, bool add, bool set),
    setEvent(e, add, set, &errmsg))

DEF_TYPE1(TorControl, bool,
    setEvents(),
    setEvents(&errmsg))

DEF_TYPE1(TorControl, bool,
    setConf(QHash<QString,QString> map),
    setConf(map, &errmsg))

DEF_TYPE1(TorControl, bool,
    setConf(QString key, QString value),
    setConf(key, value, &errmsg))

DEF_TYPE1(TorControl, bool,
    setConf(QString keyAndValue),
    setConf(keyAndValue, &errmsg))

// TODO: macros don't like template variables
// do this one by hand
//DEF_TYPE2(TorControl, bool, QHash<QString,QString>,
//    getConf(QHash<QString,QString> &map),
//    getConf(map, &errmsg))
bool 
TorControlPrototype::getConf(QHash<QString,QString> &map, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(map, errmsg);
}

// TODO: this one too
bool 
TorControlPrototype::getConf(QHash<QString,QStringList> &map, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(map, errmsg);
}

DEF_TYPE2(TorControl, bool, QString,
    getConf(QString key),
    getConf(key, ans, &errmsg))

// TODO: same as the last one with StringList
bool 
TorControlPrototype::getConf(QString key, QStringList &value, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(key, value, errmsg);
}

// TODO: idem
QVariantMap 
TorControlPrototype::getConf(const QStringList &keys, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(keys, errmsg);
}

// TODO: possibly useless
//QVariant 
//TorControlPrototype::getConf(const QString &key, QString *errmsg)
//{
//  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

//  if(obj)
//    return obj->getConf(key, errmsg);
//}

DEF_TYPE1(TorControl, QString,
    getHiddenServiceConf(const QString &key),
    getHiddenServiceConf(key, &errmsg))

DEF_TYPE1(TorControl, bool,
    saveConf(),
    saveConf(&errmsg))

// TODO: another stringlist one
bool 
TorControlPrototype::resetConf(QStringList keys, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->resetConf(keys, errmsg);
}

DEF_TYPE1(TorControl, bool,
    resetConf(QString key),
    resetConf(key, &errmsg))

// TODO: you know
QStringList 
TorControlPrototype::getRouterDescriptorText(const QString &id, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getRouterDescriptorText(id, errmsg);
}

// TODO: QVariantize RouterDescriptor
RouterDescriptor 
TorControlPrototype::getRouterDescriptor(const QString &id, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getRouterDescriptor(id, errmsg);
}

// TODO: QVariantize RouterStatus
RouterStatus 
TorControlPrototype::getRouterStatus(const QString &id, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getRouterStatus(id, errmsg);
}

// TODO: QVariantize NetworkStatus
NetworkStatus 
TorControlPrototype::getNetworkStatus(QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getNetworkStatus(errmsg);
}

// TODO: QVariantize DescriptorAnnotations
DescriptorAnnotations 
TorControlPrototype::getDescriptorAnnotations(const QString &id, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getDescriptorAnnotations(id, errmsg);
}

// TODO: QVariantize CircuitList
CircuitList 
TorControlPrototype::getCircuits(QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getCircuits(errmsg);
}

// TODO: QVariantize StreamList
StreamList 
TorControlPrototype::getStreams(QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getStreams(errmsg);
}

// TODO: QVariantize AddressMap
AddressMap 
TorControlPrototype::getAddressMap(AddressMap::AddressMapType type, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getAddressMap(type, errmsg);
}

DEF_TYPE1(TorControl, QString,
    ipToCountry(const QHostAddress &ip),
    ipToCountry(ip, &errmsg))

// TODO: migrate CircuitId
bool 
TorControlPrototype::closeCircuit(const CircuitId &circId, bool ifUnused, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->closeCircuit(circId, ifUnused, errmsg);
}

// TODO: migrate StreamId
bool 
TorControlPrototype::closeStream(const StreamId &streamId, QString *errmsg)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->closeStream(streamId, errmsg);
}
