/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#ifndef SERVICELIST_H_
#define SERVICELIST_H_
#include <QList>
#include <service.h>
class ServiceList
{
public:

  /** Default constructor. */
  ServiceList();
  /** Destructor */
  virtual ~ServiceList();
  /** Returns the list of services */
  void addService(Service service);
  /** Sets the lists of services */
  void setServices(QList<Service> services);
  /** Returns the list of services */
  QList<Service> services() const {
   return _services;
  }
  /** Writes ServiceList class data from <b>myObj</b> to the QDataStream
   * <b>out</b>. */
  friend QDataStream& operator<<(QDataStream &out, const ServiceList &myObj);
  /** Reads ServiceList class data in from the QDataStream <b>in</b> and
   populates * the <b>myObj</b> object accordingly. */
  friend QDataStream& operator>>(QDataStream &in, ServiceList &myObj);

private:

  /** The list of Services */
  QList<Service> _services;

};
Q_DECLARE_METATYPE(ServiceList);
#endif /*SERVICELIST_H_*/

