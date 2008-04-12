/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#ifndef _SERVICESETTINGS_H
#define _SERVICESETTINGS_H

#include <torcontrol.h>
#include <servicelist.h>
#include "vidaliasettings.h"
#include "exitpolicy.h"

class ServiceSettings : private VidaliaSettings
{

public:

  /** Constructor */
  ServiceSettings(TorControl *torControl);
  /** Returns the service port for a specififc service*/
  QString getVirtualPort();
  /** Set the service port for a specififc service*/
  void setVirtualPort(QString servicePort);
  /** Returns the .onion - service address for a specific service */
  QString getServiceAddress();
  /** Set the .onion - service address for a specific service */
  void setServiceAddress(QString serviceAddress);
  /** Returns the service address or hostname for a specific service */
  QString getPhysicalAddressPort();
  /** Set the service address or hostname for a specific service */
  void setPhysicalAddressPort(QString physicalAddress);
  /** Returns if the Service is enabled */
  bool isEnabled();
  /** Set the service enabled */
  void setEnabled(bool enabled);
  /** Returns a ServiceList containing all services */
  ServiceList getServices();
  /** Set ServiceList to serialise it */
  void setServices(ServiceList services);
  /** Get Service Directories */
  QString getHiddenServiceDirectories();
  /** Set all services the user wants to start and send it to the
   * Tor Controller */
  void applyServices(QString value, QString *errmsg);
  /** Unpublish all services */
  void unpublishAllServices(QString *errmsg);

private:

  /** A TorControl object used to talk to Tor. */
  TorControl* _torControl;
};

#endif

