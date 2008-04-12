/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#ifndef _SERVICEPAGE_H
#define _SERVICEPAGE_H

#include <torcontrol.h>
#include <torsettings.h>
#include <servicesettings.h>
#include <exitpolicy.h>
#include <helpbrowser.h>
#include "configpage.h"
#include "ui_servicepage.h"

class ServicePage : public ConfigPage
{
  Q_OBJECT

public:

  /** Default Constructor */
  ServicePage(QWidget *parent = 0);
  /** Default Destructor */
  ~ServicePage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();
  /** Initialize the service table */
  void initServiceTable(QMap<int, Service>* _services);

private slots:

  /** this method is called whenefer the user clicks on the 'add' Button*/
  void addService();
  /** this method is called whenefer the user clicks on the 'remove' Button*/
  void removeService();
  /** this method is called whenefer the user clicks on the 'copy' Button*/
  void copyToClipboard();
  /** this method is called whenefer the user clicks on the 'browse' Button*/
  void browseDirectory();
  /** this method is called whenefer the selects a different service*/
  void serviceSelectionChanged();
  /** this method returns a list of services by parsing the configuration
   *  string given by the tor controller */
  QList<Service> extractSingleServices(QString conf);
  /** this return a Service by parseing the configuration string
   *  of Tor and storeing its values into the object */
  Service generateService(QString serviceString);
  /** this method checks either a service is published or not */
  void startServicesInTor(QList<Service> services);
  /** this method checks either a service is published or not */
  bool isServicePublished(Service service, QList<Service> torServices);
  /** this method checks if either all services have minimal
   *  configuration or not */
  bool checkBeforeSaving(QList<Service> services);
  /** this method is called when the user finished editing a cell and 
   * it provides that only valid values are set */
  void valueChanged();

private:

  /** A TorControl object used to talk to Tor. */
  TorControl* _torControl;
  /** A TorSettings object used for saving/loading the Tor settings */
  TorSettings *_torSettings;
  /** A ServiceSettings object used to load/save the services. */
  ServiceSettings* _serviceSettings;
  /* A QMap, mapping from QString servicename to the Entity service */
  QMap<int, Service>* _services;
  /* A QList, consisting of all running services before vidalia starts*/
  QMap<QString, Service>* _torServices;
  /** Qt Designer generated object */
  Ui::ServicePage ui;
};

#endif

