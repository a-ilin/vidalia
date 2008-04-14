/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#include "service.h"

/** Default Constructor */
Service::Service() {
}

/** Constructor to create a new Service with initial settings */
Service::Service(QString serviceAddress, QString virtualPort,
 QString physicalAddressPort, QString serviceDirectory, bool enabled)
{

  _serviceAddress = serviceAddress;
  _virtualPort = virtualPort;
  _physicalAddressPort = physicalAddressPort;
  _serviceDirectory = serviceDirectory;
  _enabled = enabled;
}

/** Destructor */
Service::~Service()
{
}

/** Sets the deploy status of a service */
void Service::setEnabled(bool enabled)
{
  _enabled = enabled;
}

/** Sets the adress of a service */
void Service::setServiceAddress(QString serviceAddress)
{
  _serviceAddress = serviceAddress;
}

/** Sets the virtualPort of a service */
void Service::setVirtualPort(QString virtualPort)
{
  _virtualPort = virtualPort;
}

/** Sets the physical Adress and the local port of a service */
void Service::setPhysicalAddressPort(QString physicalAddressPort)
{
  _physicalAddressPort = physicalAddressPort;
}

/** Sets the service directory of a service */
void Service::setServiceDirectory(QString serviceDirectory)
{
  _serviceDirectory = serviceDirectory;
}

/** Sets the additional options of a service e.g. excludeNodes */
void Service::setAdditionalServiceOptions(QString options)
{
  _additionalServiceOptions = options;
}

/** Writes service class data from <b>myObj</b> to the QDataStream
 * <b>out</b>. */
QDataStream&operator<<(QDataStream &out, const Service &myObj)
{
  out << myObj.serviceAddress();
  out << myObj.virtualPort();
  out << myObj.physicalAddressPort();
  out << myObj.serviceDirectory();
  out << myObj.enabled();
  out << myObj.additionalServiceOptions();

  return out;
}

/** Reads service class data in from the QDataStream <b>in</b> and
 populates * the <b>myObj</b> object accordingly. */
QDataStream&operator>>(QDataStream &in, Service &myObj)
{
  QString serviceAddress;
  QString virtualPort;
  QString physicalAddressPort;
  QString serviceDirectory;
  bool enabled;
  QString additionalServiceOptions;

  /* Read in from the data stream */
  in >> serviceAddress >> virtualPort  >> physicalAddressPort
   >> serviceDirectory >> enabled >> additionalServiceOptions;

  /* Set the appropriate class member variables */
  myObj.setServiceAddress(serviceAddress);
  myObj.setVirtualPort(virtualPort);
  myObj.setPhysicalAddressPort(physicalAddressPort);
  myObj.setServiceDirectory(serviceDirectory);
  myObj.setEnabled(enabled);
  myObj.setAdditionalServiceOptions(additionalServiceOptions);

  /* Return the updated data stream */
  return in;
}

/** this method creates a string by concatenating the values of the service */
QString
Service::toString()
{
  return _serviceAddress +"#"+ _virtualPort +"#"+  _physicalAddressPort +
    "#"+ _serviceDirectory +"#"+  _enabled + "#"+ _additionalServiceOptions;
}

