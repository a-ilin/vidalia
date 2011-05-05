/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

#include "ServiceList.h"


/** Default constructor. */
ServiceList::ServiceList()
{
}

/** Constructor to create a new Servicelist with initial settings */
void ServiceList::addService(Service service)
{
  _services.append(service);
}

/** Destructor */
ServiceList::~ServiceList()
{
}

/* Sets the serviceList */
void ServiceList::setServices(QList<Service> services)
{
  _services = services;
}

/** Writes ServiceList class data from <b>myObj</b> to the QDataStream
 * <b>out</b>. */
QDataStream&operator<<(QDataStream &out, const ServiceList &myObj)
{
  out << myObj.services(); /* Write the services*/
  return out;
}

/** Reads ServiceList class data in from the QDataStream <b>in</b> and
 populates * the <b>myObj</b> object accordingly. */
QDataStream&operator>>(QDataStream &in, ServiceList &myObj)
{
  QList<Service> services;
  /* Read in from the data stream */
  in >> services;
  /* Set the appropriate class member variables */
  myObj.setServices(services);
  /* Return the updated data stream */
  return in;
}

