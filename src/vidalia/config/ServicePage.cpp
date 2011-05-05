/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

#include "ServicePage.h"
#include "Service.h"
#include "ServiceList.h"
#include "VMessageBox.h"
#include "ConfigDialog.h"
#include "IpValidator.h"
#include "DomainValidator.h"
#include "Vidalia.h"

#include "stringutil.h"
#include "file.h"

#include <QHeaderView>
#include <QClipboard>
#include <QFile>
#include <QTextStream>


/** Constructor */
ServicePage::ServicePage(QWidget *parent)
: ConfigPage(parent, "Services")
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  /* A QMap, mapping from the row number to the Entity for
   * all services */
  _services = new QMap<int, Service>();
  /* A QMap, mapping from the directory path to the Entity for
   * all Tor services */
  _torServices = new QMap<QString, Service>();

  ui.serviceWidget->horizontalHeader()->resizeSection(0, 150);
  ui.serviceWidget->horizontalHeader()->resizeSection(1, 89);
  ui.serviceWidget->horizontalHeader()->resizeSection(2, 100);
  ui.serviceWidget->horizontalHeader()->resizeSection(3, 120);
  ui.serviceWidget->horizontalHeader()->resizeSection(4, 60);
  ui.serviceWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  ui.serviceWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
  ui.serviceWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
  ui.serviceWidget->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
  ui.serviceWidget->verticalHeader()->hide();

  connect(ui.addButton, SIGNAL(clicked()), this, SLOT(addService()));
  connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(removeService()));
  connect(ui.copyButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
  connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(browseDirectory()));
  connect(ui.serviceWidget, SIGNAL(itemClicked(QTableWidgetItem*)),
          this, SLOT(serviceSelectionChanged()));
  connect(ui.serviceWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
          this, SLOT(valueChanged()));
}

/** Destructor */
ServicePage::~ServicePage()
{
  delete _services;
  delete _torServices;
}

/** Called when the user changes the UI translation. */
void
ServicePage::retranslateUi()
{
  ui.retranslateUi(this);
}

/** Saves changes made to settings on the Server settings page. */
bool
ServicePage::save(QString &errmsg)
{
  ServiceSettings serviceSettings(Vidalia::torControl());
  QList<Service> serviceList;
  QList<Service> publishedServices;
  int index = 0;

  while(index < ui.serviceWidget->rowCount()) {
    QString address = ui.serviceWidget->item(index,0)->text();
    QString virtualPort = ui.serviceWidget->item(index,1)->text();
    QString physicalAddress = ui.serviceWidget->item(index,2)->text();
    QString directoryPath = ui.serviceWidget->item(index,3)->text();
    bool enabled = _services->value(index).enabled();
    Service temp(address, virtualPort, physicalAddress, directoryPath,
                 enabled);
    temp.setAdditionalServiceOptions(
      _services->value(ui.serviceWidget->currentRow()).additionalServiceOptions());
    serviceList.push_back(temp);
    if(enabled) {
      publishedServices.push_back(temp);
    }
    index++;
  }

  bool save = checkBeforeSaving(serviceList);
  if(save) {
    ServiceList sList;
    if(serviceList.size() > 0) {
      sList.setServices(serviceList);
    } else {
      _services = new QMap<int, Service>();
      sList.setServices(_services->values());
    }
    serviceSettings.setServices(sList);
    if(publishedServices.size() > 0) {
      startServicesInTor(publishedServices);
    } else {
      QString errmsg1 = tr("Error while trying to unpublish all services");
      QString &errmsg = errmsg1;
      serviceSettings.unpublishAllServices(&errmsg);
    }
    return true;
  } else {
    errmsg = tr("Please configure at least a service directory and a virtual "
                "port for each service you want to save. Remove the other ones.");
    return false;
  }
}

/** this method checks if either all services have minimal
 *  configuration or not */
bool
ServicePage::checkBeforeSaving(QList<Service> serviceList)
{
  bool result = true;
  foreach(Service s, serviceList) {
    if(s.serviceDirectory().isEmpty() || s.virtualPort().isEmpty()) {
      result = false;
      break;
    }
  }
  return result;
}

/** this method generates the configuration string for a list of services */
void
ServicePage::startServicesInTor(QList<Service> services)
{
  ServiceSettings serviceSettings(Vidalia::torControl());
  QString serviceConfString;
  QString errmsg = "Error while trying to publish services.";
  QListIterator<Service> it(services);

  while(it.hasNext()) {
    Service temp = it.next();
    serviceConfString.append("hiddenservicedir=" +
                             string_escape(temp.serviceDirectory()) + " ");
    serviceConfString.append("hiddenserviceport=" +
     string_escape(temp.virtualPort() +
     (temp.physicalAddressPort().isEmpty() ? "" : " " +
      temp.physicalAddressPort())));
    serviceConfString.append(" " + temp.additionalServiceOptions());
  }
  serviceSettings.applyServices(serviceConfString, &errmsg);
}

/** Loads previously saved settings */
void
ServicePage::load()
{
  ServiceSettings serviceSettings(Vidalia::torControl());
  QList<Service> torServiceList;

  ui.removeButton->setEnabled(false);
  ui.copyButton->setEnabled(false);
  ui.browseButton->setEnabled(false);
  // get all services
  _services->clear();
  _torServices->clear();

  QString torConfigurationString = serviceSettings.getHiddenServiceDirectories();
  torServiceList = extractSingleServices(torConfigurationString);
  QList<Service> completeList = torServiceList;
  // the services stored with vidalia
  ServiceList serviceList = serviceSettings.getServices();
  QList<Service> serviceSettingsList = serviceList.services();
  QListIterator<Service> it(serviceSettingsList);
  // check whether a service is already in the list because he is published
  while(it.hasNext()) {
    Service temp = it.next();
    if(isServicePublished(temp, torServiceList) == false) {
      completeList.push_back(temp);
    }
  }
  // generate the _services data structure used during vidalia session
  QListIterator<Service> it2(completeList);
  int index = 0;
  while (it2.hasNext()) {
    Service tempService = it2.next();
    _services->insert(index, tempService);
    index++;
  }
  initServiceTable(_services);
}

/** this method returns a list of services by parsing the configuration
 *  string given by the tor controller */
QList<Service>
ServicePage::extractSingleServices(QString conf)
{
  QList<Service> list;
  QStringList strList = conf.split("250 HiddenServiceDir");
  strList.removeFirst();
  QListIterator<QString> it(strList);
  //for each service directory splitted string = service
  while(it.hasNext()) {
    QString temp = it.next();
    list.push_back(generateService(temp));
  }
  return list;
}

/** this return a Service by parseing the configuration string
 *  of Tor and storeing its values into the object */
Service
ServicePage::generateService(QString s)
{
  QString additionalOptions = s;
  // remove directory
  int index = additionalOptions.indexOf("250",1);
  additionalOptions.remove(0, index+4);
  // remove the first appearance of the port
  int startindex = additionalOptions.indexOf("hiddenserviceport", 0,
                                             Qt::CaseInsensitive);
  int endindex = additionalOptions.indexOf("250", startindex);
  if(endindex != -1) {
    additionalOptions.remove(startindex, (endindex-startindex)+4);
    //remove all appearances of "250"
    while(additionalOptions.contains("250")) {
      int i = additionalOptions.indexOf("250", 0);
      additionalOptions.remove(i, 4);
    }
    // prepare for correct quotation
    if (!additionalOptions.endsWith('\n')) {
      additionalOptions.append("\n");
    }
    //quote the values
    int j = additionalOptions.indexOf("=", 0);
    while(j != -1) {
      additionalOptions.insert(j+1, "\"");
      int end = additionalOptions.indexOf("\n", j);
      additionalOptions.insert(end, "\"");
      j = additionalOptions.indexOf("=", end);
    }
    //replace the line brakes with a space and create one single line
    additionalOptions.replace(QString("\n"), QString(" "));
  } else {
      additionalOptions = "";
  }

  QString address, virtualPort, physAddressPort, serviceDir;
  // service directory
  QStringList strList = s.split("\n");
  QString tempServiceDir = strList.first().trimmed();
  serviceDir = tempServiceDir.remove(0, 1);
  //virtual port
  QStringList strList2 = s.split("HiddenServicePort");
  strList2.removeFirst();
  QStringList strList3 = strList2.first().split("\n");
  QStringList strList4 = strList3.first().split(" ");
  if(strList4.size() > 0) {
    QString tempVirtualPort = strList4.first();
    virtualPort = tempVirtualPort.remove(0, 1);
    strList4.removeFirst();
    //physical address:port
    if(!strList4.isEmpty()) {
      physAddressPort = strList4.first().trimmed();
    }
  } else {
    QString tempVirtualPort = strList3.first();
    virtualPort = tempVirtualPort.remove(0, 1);
  }
  //get .onion address
  QString serviceHostnameDir = serviceDir;
  serviceHostnameDir.append("/");
  serviceHostnameDir.append("hostname");
  QFile file(serviceHostnameDir);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    address = "[Directory not found]";
  } else {
    QTextStream in(&file);
    QString hostname;
    while (!in.atEnd()) {
      hostname.append(in.readLine());
    }
    address = hostname;
  }
  Service service(address, virtualPort, physAddressPort, serviceDir, true);
  service.setAdditionalServiceOptions(additionalOptions);
  _torServices->insert(serviceDir, service);
  return service;
}

/** this method checks either a service is published or not */
bool
ServicePage::isServicePublished(Service service, QList<Service> torServices)
{
  QListIterator<Service> it(torServices);
  while(it.hasNext()) {
    Service temp = it.next();
    if(temp.serviceDirectory().compare(service.serviceDirectory()) == 0) {
      return true;
    }
  }
  return false;
}

/** this method creates/displays the values for each service
 *  shown in the service listing */
void
ServicePage::initServiceTable(QMap<int, Service>* services)
{
  // clean the widget
  int rows = ui.serviceWidget->rowCount();
  for(int i = 0; i < rows; i++) {
    ui.serviceWidget->removeRow(0);
  }
  //for each service
  int index = 0;
  while(index < services->size()) {
    Service tempService = services->value(index);
    ui.serviceWidget->insertRow(index);
    QTableWidgetItem *cboxitem = new QTableWidgetItem();
    cboxitem->setFlags(Qt::ItemIsSelectable);
    QTableWidgetItem *addressitem = new QTableWidgetItem();
    addressitem->setFlags(Qt::ItemIsSelectable);
    if(tempService.serviceAddress().length() < 0) {
      addressitem->setText(tempService.serviceAddress());
    } else {
      QString serviceHostnameDir = tempService.serviceDirectory();
      serviceHostnameDir.append("/");
      serviceHostnameDir.append("hostname");
      QFile file(serviceHostnameDir);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        addressitem->setText("[Directory not found]");
      } else {
        QTextStream in(&file);
        QString hostname;
        while (!in.atEnd()) {
          hostname.append(in.readLine());
        }
        addressitem->setText(hostname);
        tempService.setServiceAddress(hostname);
      }
    }
    addressitem->setData(32, addressitem->text());
    QTableWidgetItem *serviceDir =
        new QTableWidgetItem(tempService.serviceDirectory(), 0);
    serviceDir->setData(32, tempService.serviceDirectory());
    QTableWidgetItem* virtualportitem =
        new QTableWidgetItem(tempService.virtualPort(), 0);
    virtualportitem->setData(32, tempService.virtualPort());
    QTableWidgetItem* targetitem =
        new QTableWidgetItem(tempService.physicalAddressPort(),0);
    targetitem->setData(32, tempService.physicalAddressPort());
    if(tempService.enabled()) {
      cboxitem->setCheckState(Qt::Checked);
      serviceDir->setFlags(Qt::ItemIsSelectable);
    } else {
      cboxitem->setCheckState(Qt::Unchecked);
    }
    cboxitem->setTextAlignment(Qt::AlignCenter);
    ui.serviceWidget->setItem(index, 0, addressitem);
    ui.serviceWidget->setItem(index, 1, virtualportitem);
    ui.serviceWidget->setItem(index, 2, targetitem);
    ui.serviceWidget->setItem(index, 3, serviceDir);
    ui.serviceWidget->setItem(index, 4, cboxitem);
    index++;
  }
}

/** this method is called when the user clicks the "Add"-Button
 *  it generates a new empty table entrie(row) */
void
ServicePage::addService()
{
  int rows = ui.serviceWidget->rowCount();
  ui.serviceWidget->insertRow(rows);
  QTableWidgetItem *address = new QTableWidgetItem("["+tr("Created by Tor")+"]");
  address->setFlags(Qt::ItemIsSelectable);
  QTableWidgetItem *dummy = new QTableWidgetItem();
  QTableWidgetItem *dummy2 = new QTableWidgetItem();
  QTableWidgetItem *dummy3 = new QTableWidgetItem();
  QTableWidgetItem *cboxitem = new QTableWidgetItem();
  cboxitem->setFlags(Qt::ItemIsSelectable);
  cboxitem->setCheckState(Qt::Checked);
  ui.serviceWidget->setItem(rows, 0, address);
  ui.serviceWidget->setItem(rows, 1, dummy);
  ui.serviceWidget->setItem(rows, 2, dummy2);
  ui.serviceWidget->setItem(rows, 3, dummy3);
  ui.serviceWidget->setItem(rows, 4, cboxitem);
  Service s;
  s.setEnabled(true);
  _services->insert(rows, s);
}

/** this method is called when the user clicks the "Remove"-Button
 *  it removes a service/row of the service listing */
void
ServicePage::removeService()
{
  int rows = ui.serviceWidget->rowCount();
  int selrow = ui.serviceWidget->currentRow();
  if(selrow < 0 || selrow >= _services->size()) {
    VMessageBox::warning(this, tr("Error"), tr("Please select a Service."),
                         VMessageBox::Ok);
    return;
  } else {
    ui.serviceWidget->removeRow(selrow);
    //decrease all other service keys
    for(int i = 0; i < (rows-selrow-1); i++) {
      int index = i+selrow;
      Service s = _services->take(index+1);
      _services->insert(index, s);
    }
  }
  serviceSelectionChanged();
}

/** this method is called when the user clicks on the "Copy"-Button, it
 *  copies the .onion-Address of the selected service into the clipboard */
void
ServicePage::copyToClipboard()
{
  int selrow = ui.serviceWidget->currentRow();
  if(selrow < 0 || selrow >= _services->size()) {
    VMessageBox::warning(this, tr("Error"), tr("Please select a Service."),
                         VMessageBox::Ok);
    return;
  } else {
    QString onionAddress = ui.serviceWidget->item(selrow,0)->text();
    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText;
    QTableWidgetItem* selectedItem = ui.serviceWidget->item(selrow,0);
    clipboardText.append(selectedItem->text());
    clipboard->setText(clipboardText);
  }
}

/** this method is called when the user clicks on the "Brows"-Button it opens
 *  a QFileDialog to choose a service directory */
void
ServicePage::browseDirectory()
{
  int selrow = ui.serviceWidget->currentRow();
  if(selrow < 0 || selrow >= _services->size()) {
    VMessageBox::warning(this, tr("Error"), tr("Please select a Service."),
                         VMessageBox::Ok);
    return;
  } else {
    QString dirname =
      QFileDialog::getExistingDirectory(this,
                                        tr("Select Service Directory"), "",
                                        QFileDialog::ShowDirsOnly
                                          | QFileDialog::DontResolveSymlinks);

    if (dirname.isEmpty()) {
      return;
    }
    ui.serviceWidget->item(selrow,3)->setText(dirname);
    Service s = _services->take(selrow);
    s.setServiceDirectory(dirname);
    _services->insert(selrow, s);
  }
}

/** this method is called when the selects an other tablewidgetitem */
void
ServicePage::serviceSelectionChanged()
{
  bool emptyTable = false;
  if(ui.serviceWidget->rowCount() > 0) {
    ui.removeButton->setEnabled(true);
    ui.copyButton->setEnabled(true);
    ui.browseButton->setEnabled(true);
  } else {
    ui.removeButton->setEnabled(false);
    ui.copyButton->setEnabled(false);
    ui.browseButton->setEnabled(false);
    emptyTable = true;
  }
  int currentRow = ui.serviceWidget->currentRow();
  if(emptyTable == false) {
    QTableWidgetItem* item = ui.serviceWidget->item(currentRow, 0);
    if(item != NULL) {
      bool b = item->text().contains(".onion");
      ui.copyButton->setEnabled(b);
    }
  }
  
  QString selDir = _services->value(ui.serviceWidget->currentRow()).
                                    serviceDirectory();
  QList<QString> strList =  _torServices->keys();
  if(selDir.length() > 0) {
    QListIterator<QString> it(strList);
    while(it.hasNext()) {
      QString temp = it.next();
      if(selDir.compare(temp) == 0) {
        ui.browseButton->setEnabled(false);
        break;
      }
    }
  }
  // if the user has clicked on the checkbox cell
  if(ui.serviceWidget->currentColumn() == 4) {
    Service service = _services->take(currentRow);
    QTableWidgetItem* item = ui.serviceWidget->item(currentRow,4);
    if(service.enabled()) {
      item->setCheckState(Qt::Unchecked);
      service.setEnabled(false);
    } else {
      item->setCheckState(Qt::Checked);
      service.setEnabled(true);
    }
    _services->insert(currentRow, service);
  }
}

/** this method is called when the user finished editing a cell and it provides
 *  that only valid values are set */
void
ServicePage::valueChanged()
{
  int pos = 0;
  QIntValidator* portValidator = new QIntValidator(1, 65535, this);
  DomainValidator* domainValidator = new DomainValidator(this);
  IpValidator* ipValidator = new IpValidator(this);
  QTableWidgetItem* item = ui.serviceWidget->currentItem();
  if (item == NULL || item->text() == NULL || item->text().length() == 0) {
    // nothing to validate here
    return;
  }
  QString text = item->text();
  switch (item->column()) {
    case 1: // virtual port
      if(portValidator->validate(text, pos) == QValidator::Acceptable) {
        // correct data; buffer value in user role 32
        item->setData(32, text);
      } else {
        //incorrect data; restore value from user role 32
        VMessageBox::warning(this, tr("Error"),
            tr("Virtual Port may only contain valid port numbers [1..65535]."),
            VMessageBox::Ok);
        item->setText(item->data(32).toString());
      }
      break;
    case 2: // target
      if(text.contains(":")) {
        // check for <address>:<port>
        QStringList strList = text.split(":");
        if (strList.size() != 2) {
          goto invalid;
        }
        QString address = strList.at(0);
        QString port = strList.at(1);
        if((address.compare("localhost") != 0 &&
          ipValidator->validate(address, pos) != QValidator::Acceptable &&
          domainValidator->validate(address, pos) != QValidator::Acceptable) ||
          portValidator->validate(port, pos) != QValidator::Acceptable) {
          goto invalid;
        }
      } else { // either <address> or <port>
        if (text.compare("localhost") != 0 &&
          ipValidator->validate(text, pos) != QValidator::Acceptable &&
          domainValidator->validate(text, pos) != QValidator::Acceptable &&
          portValidator->validate(text, pos) != QValidator::Acceptable) {
          goto invalid;
        }
      }
      goto valid;
 invalid:
      VMessageBox::warning(this, tr("Error"),
          tr("Target may only contain address:port, address, or port."),
          VMessageBox::Ok);
      item->setText(item->data(32).toString());
      break;
 valid:
      item->setData(32, text);
      break;
    case 3: // service directory
      // compare with directories of other enabled services
      for (int index = 0; index < ui.serviceWidget->rowCount(); index++) {
        // skip own row
        if(index == item->row()) {
          continue;
        }
        QTableWidgetItem* compareWith = ui.serviceWidget->item(index, 3);
        if(compareWith != NULL) {
          QString actualDir = compareWith->text();
          if(actualDir.length() > 0 && text.compare(actualDir) == 0) {
            // service directory already in use
            VMessageBox::warning(this, tr("Error"),
                tr("Directory already in use by another service."),
                VMessageBox::Ok);
            item->setText(item->data(32).toString());
            return;
          }
        }
      }
      // correct data; buffer value in user role 32
      item->setData(32, text);
      break;
  }
}

