/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#include <stringutil.h>
#include "servicesettings.h"
#include "torsettings.h"

/* Service Settings */
#define SETTING_SERVICE_VIRTUAL_PORT "Service/VirtualPort"
#define SETTING_SERVICE_ADDRESS "Service/ServiceAddress"
#define SETTING_SERVICE_PHYSICAL_ADDRESS "Service/ServicePhysicalAddress"
#define SETTING_SERVICE_ENABLED "Service/Enabled"
#define SETTING_TOR_SERVICES "Service/Services"

/** Constructor.
 * \param torControl a TorControl object used to read and apply the Service
 * configuration settings.
 */
ServiceSettings::ServiceSettings(TorControl *torControl)
{
  _torControl = torControl;
  setDefault(SETTING_SERVICE_VIRTUAL_PORT , 0);
  setDefault(SETTING_SERVICE_PHYSICAL_ADDRESS, "127.0.0.1:0");
  setDefault(SETTING_SERVICE_ENABLED, "true");
}

/** Set ServiceList to serialise it */
void
ServiceSettings::setServices(ServiceList service)
{
  QStringList serviceList;
  if(service.services().size() > 0) {
    QList<Service> services = service.services();
    foreach (Service tempService, services) {
      serviceList << tempService.toString();
    }
  }
  setValue(SETTING_TOR_SERVICES, serviceList);
}

/** Get  serialised ServiceList */
ServiceList
ServiceSettings::getServices()
{
  QString address,virtualPort,physAddrPort,serviceDir,enabledS,additionalData;
  bool enabled = false;
  QStringList stringList;
  ServiceList services;

  stringList = value(SETTING_TOR_SERVICES).toStringList();
  foreach (QString s, stringList) {
    QStringList skippedList = s.split("#");
    address = skippedList.first();
    skippedList.removeFirst();
    virtualPort = skippedList.first();
    skippedList.removeFirst();
    physAddrPort = skippedList.first();
    skippedList.removeFirst();
    serviceDir = skippedList.first();
    skippedList.removeFirst();
    enabledS = skippedList.first();
    skippedList.removeFirst();
    additionalData = skippedList.first();
    if(enabledS.compare("x1") == 0) {
      enabled = true;
    }
    Service service(address, virtualPort, physAddrPort, serviceDir, enabled);
    service.setAdditionalServiceOptions(additionalData);
    services.addService(service);
  }
  return services;
}

/** Returns the virtual port for a specific service*/
QString
ServiceSettings::getVirtualPort()
{
  QString port = value(SETTING_SERVICE_VIRTUAL_PORT).toString();
  return port;
}

/** Set the virtual port for a specific service*/
void
ServiceSettings::setVirtualPort(QString servicePort)
{
  setValue(SETTING_SERVICE_VIRTUAL_PORT, servicePort);
}

/** Returns the .onion - service address for a specific service */
QString
ServiceSettings::getServiceAddress()
{
  QString addr = value(SETTING_SERVICE_ADDRESS).toString();
  return addr;
}

/** Set the .onion - service address or hostname for a specific service */
void
ServiceSettings::setServiceAddress(QString addr)
{
  setValue(SETTING_SERVICE_ADDRESS, addr);
}

/** Returns the physical address for a specific service */
QString
ServiceSettings::getPhysicalAddressPort()
{
  QString addr = value(SETTING_SERVICE_PHYSICAL_ADDRESS).toString();
  return addr;
}

/** Set the physical address or hostname for a specific service */
void
ServiceSettings::setPhysicalAddressPort(QString addr)
{
  setValue(SETTING_SERVICE_PHYSICAL_ADDRESS, addr);
}

/** Returns if the Service is enabled */
bool
ServiceSettings::isEnabled()
{
  return value(SETTING_SERVICE_ENABLED).toBool();
}

/** Set the service enabled */
void
ServiceSettings::setEnabled(bool boolean)
{
  setValue(SETTING_SERVICE_ENABLED, boolean);
}

/** Get all service directories from Tor */
QString
ServiceSettings::getHiddenServiceDirectories()
{
  /*XXX: Domenik: Why does this always try to getconf hiddenserviceoptions
   * even if the socket is not connected? */
  QString value =  _torControl->getHiddenServiceConf("hiddenserviceoptions");
  return value;
}

/** Set all services the user wants to start and send it to the
 * Tor Controller*/
void
ServiceSettings::applyServices(QString value, QString *errmsg)
{
  _torControl->setConf(value, errmsg);
  _torControl->saveConf(errmsg);
}

/** Unpublish all HiddenServices */
void
ServiceSettings::unpublishAllServices(QString *errmsg)
{
  _torControl->resetConf("HiddenServiceDir", errmsg);
  _torControl->saveConf(errmsg);
}

