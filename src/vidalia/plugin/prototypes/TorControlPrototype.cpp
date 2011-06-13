#include "TorControlPrototype.h"

TorControlPrototype::TorControlPrototype()
  : QObject(), QScriptable() {}

void 
TorControlPrototype::start(const QString &tor, const QStringList &args)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    obj->start(tor, args);
}

bool 
TorControlPrototype::stop(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->stop(errmsg);
}

bool 
TorControlPrototype::isRunning()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->isRunning();
}

bool 
TorControlPrototype::isVidaliaRunningTor()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->isVidaliaRunningTor();
}

void 
TorControlPrototype::closeTorStdout()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    obj->closeTorStdout();
}

void 
TorControlPrototype::connect(const QHostAddress &address, quint16 port)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    obj->connect(address, port);
}

void 
TorControlPrototype::connect(const QString &path)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    obj->connect(path);
}

void 
TorControlPrototype::disconnect()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    obj->disconnect();
}

bool 
TorControlPrototype::isConnected()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->isConnected();
}

bool 
TorControlPrototype::authenticate(const QByteArray cookie, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->authenticate(cookie, errmsg);
}

bool 
TorControlPrototype::authenticate(const QString &password = QString(), QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->authenticate(password, errmsg);
}

ProtocolInfo 
TorControlPrototype::protocolInfo(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->protocolInfo(errmsg);
}

BootstrapStatus 
TorControlPrototype::bootstrapStatus(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->bootstrapStatus(errmsg);
}

bool 
TorControlPrototype::isCircuitEstablished()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->isCircuitEstablished();
}

bool 
TorControlPrototype::getInfo(QHash<QString,QString> &map, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getInfo(map, errmsg);
}

bool 
TorControlPrototype::getInfo(QString key, QString &val, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getInfo(key, val, errmsg);
}

QVariantMap 
TorControlPrototype::getInfo(const QStringList &keys, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getInfo(keys, errmsg);
}

QVariant 
TorControlPrototype::getInfo(const QString &key, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getInfo(key, errmsg);
}

bool 
TorControlPrototype::signal(TorSignal::Signal sig, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->signal(sig, errmsg);
}

QHostAddress 
TorControlPrototype::getSocksAddress(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getSocksAddress(errmsg);
}

QStringList 
TorControlPrototype::getSocksAddressList(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getSocksAddressList(errmsg);
}

quint16 
TorControlPrototype::getSocksPort(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getSocksPort(errmsg);
}

QList
TorControlPrototype::<quint16> getSocksPortList(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->quint16> getSocksPortList(errmsg);
}

QString 
TorControlPrototype::getTorVersionString()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getTorVersionString();
}

quint32 
TorControlPrototype::getTorVersion()
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getTorVersion();
}

bool 
TorControlPrototype::setEvent(TorEvents::Event e, bool add = true, bool set = true, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->setEvent(e, add, set, errmsg);
}

bool 
TorControlPrototype::setEvents(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->setEvents(errmsg);
}

bool 
TorControlPrototype::setConf(QHash<QString,QString> map, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->setConf(map, errmsg);
}

bool 
TorControlPrototype::setConf(QString key, QString value, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    returb obj->setConf(key, value, errmsg);
}

bool 
TorControlPrototype::setConf(QString keyAndValue, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->setConf(keyAndValue, errmsg);
}

bool 
TorControlPrototype::getConf(QHash<QString,QString> &map, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(map, errmsg);
}

bool 
TorControlPrototype::getConf(QHash<QString,QStringList> &map, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(map, errmsg);
}

bool 
TorControlPrototype::getConf(QString key, QString &value, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(key, value, errmsg);
}

bool 
TorControlPrototype::getConf(QString key, QStringList &value, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(key, value, errmsg);
}

QVariantMap 
TorControlPrototype::getConf(const QStringList &keys, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(keys, errmsg);
}

QVariant 
TorControlPrototype::getConf(const QString &key, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getConf(key, errmsg);
}

QString 
TorControlPrototype::getHiddenServiceConf(const QString &key, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getHiddenServiceConf(key, errmsg);
}

bool 
TorControlPrototype::saveConf(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->saveConf(errmsg);
}

bool 
TorControlPrototype::resetConf(QStringList keys, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->resetConf(keys, errmsg);
}

bool 
TorControlPrototype::resetConf(QString key, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->resetConf(key, errmsg);
}

QStringList 
TorControlPrototype::getRouterDescriptorText(const QString &id, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getRouterDescriptorText(id, errmsg);
}

RouterDescriptor 
TorControlPrototype::getRouterDescriptor(const QString &id, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getRouterDescriptor(id, errmsg);
}

RouterStatus 
TorControlPrototype::getRouterStatus(const QString &id, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getRouterStatus(id, errmsg);
}

NetworkStatus 
TorControlPrototype::getNetworkStatus(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getNetworkStatus(errmsg);
}

DescriptorAnnotations 
TorControlPrototype::getDescriptorAnnotations(const QString &id, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getDescriptorAnnotations(id, errmsg);
}

CircuitList 
TorControlPrototype::getCircuits(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getCircuits(errmsg);
}

StreamList 
TorControlPrototype::getStreams(QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getStreams(errmsg);
}

AddressMap 
TorControlPrototype::getAddressMap(AddressMap::AddressMapType type = AddressMap::AddressMapAll, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->getAddressMap(type, errmsg);
}

QString 
TorControlPrototype::ipToCountry(const QHostAddress &ip, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->ipToCountry(ip, errmsg);
}

bool 
TorControlPrototype::closeCircuit(const CircuitId &circId, bool ifUnused = false, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->closeCircuit(circId, ifUnused, errmsg);
}

bool 
TorControlPrototype::closeStream(const StreamId &streamId, QString *errmsg = 0)
{
  TorControl *obj = qscriptvalue_cast<TorControl *>(thisObject());

  if(obj)
    return obj->closeStream(streamId, errmsg);
}
