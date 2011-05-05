/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

#ifndef _SERVICE_H
#define _SERVICE_H

#include <QString>
#include <QList>
#include <QMetaType>


class Service
{
public:
  /** Default constructor. */
  Service();
  /** Constructor to create a new Service with initial settings */
  Service(QString serviceAddress, QString virtualPort,
   QString physicalAddressPort, QString serviceDirectory, bool enabled);
  /** Destructor */
  virtual ~Service();
  /** Returns the service Adress of the service */
  QString serviceAddress() const { return _serviceAddress; }
  /** Returns the listeningPort of the service */
  QString virtualPort() const { return _virtualPort; }
  /** Returns the physical Adresse and the local Port of the service */
  QString physicalAddressPort() const { return _physicalAddressPort; }
  /** Returns the service directory of the service */
  QString serviceDirectory() const { return _serviceDirectory; }
  /** Returns the deployed status of a service */
  bool enabled() const { return _enabled; }
  /** Returns the additional options of a service e.g. excludeNodes */
  QString additionalServiceOptions() const
   { return _additionalServiceOptions; }
  /** Sets the adress of a service */
  void setServiceAddress(QString serviceAddress);
  /** Sets the listening port of a service */
  void setVirtualPort(QString virtualPort);
  /** Sets the physical Adress and the local Port of a service */
  void setPhysicalAddressPort(QString physicalAddressPort);
  /** Sets the service directory of a service */
  void setServiceDirectory(QString serviceDirectory);
  /** Sets the deployed status a service */
  void setEnabled(bool enabled);
  /** Sets the additional options of a service e.g. excludeNodes */
  void setAdditionalServiceOptions(QString options);
  /** Writes service class data from <b>myObj</b> to the QDataStream
  * <b>out</b>. */
  friend QDataStream& operator<<(QDataStream &out, const Service &myObj);
  /** Reads service class data in from the QDataStream <b>in</b> and
  populates * the <b>myObj</b> object accordingly. */
  friend QDataStream& operator>>(QDataStream &in, Service &myObj);
  /** this method creates a string by concatenating the values of the service */
  QString toString();

private:
  /** The adress of the service */
  QString _serviceAddress;
  /** The listening Port of the service */
  QString _virtualPort;
  /** The physical Adress and the local port of teh service */
  QString _physicalAddressPort;
  /** the directory of the service */
  QString _serviceDirectory;
  /** The Enabled status of the service */
  bool _enabled;
  /** Some additional service options, not configured/displayed by Vidalia */
  QString _additionalServiceOptions;

};
Q_DECLARE_METATYPE(Service);
#endif /*SERIVCE_H_*/

