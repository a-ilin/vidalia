/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file netviewer.h
** \version $Id$
** \brief Displays a map of the Tor network and the user's circuits
*/

#ifndef _NETVIEWER_H
#define _NETVIEWER_H

#include <QMainWindow>
#include <QStringList>
#include <QEvent>
#include <QTimer>
#include <QHash>
#include <torcontrol.h>
#include <vidaliawindow.h>

#include "geoipresolver.h"
#include "tormapwidget.h"
#include "ui_netviewer.h"


class NetViewer : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Default constructor */
  NetViewer(QWidget* parent = 0);

public slots:
  /** Displays the network map window. */
  void showWindow();
  /** Loads a list of current circuits and streams. */
  void loadConnections();
  /** Adds <b>circuit</b> to the list and the map */
  void addCircuit(const Circuit &circuit);
  /** Adds <b>stream</b> to the list of circuits, under the appropriate
   * circuit. */
  void addStream(const Stream &stream);
  /** Clears all known information */
  void clear();

protected:
  /** Called to deliver a NEWDESC event from Tor. */
  void customEvent(QEvent *event);
  /** Called when the user changes the UI translation. */
  void retranslateUi();

private slots:
  /** Called when the user selects the "Help" action on the toolbar. */
  void help();
  /** Called when the user selects the "Refresh" action on the toolbar */
  void refresh();
  /** Called when the user selects a circuit on the circuit list */
  void circuitSelected(const Circuit &circuit);
  /** Called when an IP has been resolved to geographic information. */
  void resolved(int id, const QList<GeoIp> &geoips);
  /** Called when the user selects one or more routers in the list. */
  void routerSelected(const QList<RouterDescriptor> &routers);
  /** Handles when we get connected to Tor network */
  void onAuthenticated();
  /** Handles when we get disconnected from Tor network */
  void onDisconnected();
  /** Resolves IP addresses in the resolve queue to geographic information. */
  void resolve();
  /** Called when the user selects a router on the network map. Displays a 
   * dialog with detailed information for the router specified by
   * <b>id</b>.*/
  void displayRouterInfo(const QString &id);
  /** Called when the user clicks the "Zoom In" button. */
  void zoomIn();
  /** Called when the user clicks the "Zoom Out" button. */
  void zoomOut();

private:
  /** Adds an IP address to the resolve queue and updates the queue timers. */
  void addToResolveQueue(const QHostAddress &ip, const QString &id);
  /** Retrieves a list of all running routers from Tor and their descriptors,
   * and adds them to the RouterListWidget. */
  void loadNetworkStatus();
  /** Loads a list of address mappings from Tor. */
  void loadAddressMap();
  /** Adds a router to our list of servers and retrieves geographic location
   * information for the server. */
  void addRouter(const RouterDescriptor &rd);
  /** Called when a NEWDESC event arrives. Retrieves new router descriptors
   * for the router identities given in <b>ids</b> and updates the router list
   * and network map. */
  void newDescriptors(const QStringList &ids);

  /** TorControl object used to talk to Tor. */
  TorControl* _torControl;
  /** Timer that fires once an hour to update the router list. */
  QTimer _refreshTimer;
  /** TorMapWidget that displays the map. */
  TorMapWidget* _map;
  /** GeoIpResolver used to geolocate routers by IP address. */
  GeoIpResolver _geoip;
  /** Queue for IPs pending resolution to geographic information. */
  QList<QHostAddress> _resolveQueue;
  /** Maps pending GeoIP requests to server IDs. */
  QHash<QString, QString> _resolveMap;
  /** Stores a list of address mappings from Tor. */
  AddressMap _addressMap;
  /** Timer used to delay GeoIP requests for MIN_RESOLVE_QUEUE_DELAY
   * milliseconds after we've inserted the last item into the queue. */
  QTimer _minResolveQueueTimer;
  /** Timer used to limit the delay of GeoIP requests to
   * MAX_RESOLVE_QUEUE_DELAY milliseconds after inserting the first item 
   * into the queue. */
  QTimer _maxResolveQueueTimer;
  
  /** Qt Designer generated object **/
  Ui::NetViewer ui;
};

#endif

