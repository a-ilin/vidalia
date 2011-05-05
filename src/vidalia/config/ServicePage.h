/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

#ifndef _SERVICEPAGE_H
#define _SERVICEPAGE_H

#include "ui_ServicePage.h"
#include "ConfigPage.h"
#include "TorSettings.h"
#include "ServiceSettings.h"
#include "ExitPolicy.h"

#include "TorControl.h"


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
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private slots:
  /** Called whenever the user clicks on the 'add' button. */
  void addService();
  /** Called whenever the user clicks on the 'remove' button. */
  void removeService();
  /** Called whenever the user clicks on the 'copy' button. */
  void copyToClipboard();
  /** Called whenever the user clicks on the 'browse' button. */
  void browseDirectory();
  /** Called whenever the user selects a different service. */
  void serviceSelectionChanged();
  /** Returns a list of services by parsing the configuration string given
   * by the Tor controller. */
  QList<Service> extractSingleServices(QString conf);
  /** Returns a Service by parsing the configuration string from Tor and
   * storing its values into the Service object. */
  Service generateService(QString serviceString);
  /** Starts all services in <b>services</b>, with Tor. */
  void startServicesInTor(QList<Service> services);
  /** Returns true if <b>service</b> is published. */
  bool isServicePublished(Service service, QList<Service> torServices);
  /** Returns true if all services have the required minimal configuration. */
  bool checkBeforeSaving(QList<Service> services);
  /** Called when the user finished editing a cell and checks that only valid
   * values are set. */
  void valueChanged();

private:
  /** A QMap, mapping from the row number in the table to the service Entity */
  QMap<int, Service>* _services;
  /** A QList, consisting of all running services before vidalia starts */
  QMap<QString, Service>* _torServices;

  /** Qt Designer generated object */
  Ui::ServicePage ui;
};

#endif

