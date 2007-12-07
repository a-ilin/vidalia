/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file netviewer.h
 * \version $Id$
 * \brief Displays a map of the Tor network and the user's circuits
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
  void addCircuit(Circuit circuit);
  /** Adds <b>stream</b> to the list of circuits, under the appropriate
   * circuit. */
  void addStream(Stream stream);
  /** Clears all known information */
  void clear();

protected:
  /** Called to deliver a NEWDESC event from Tor. */
  void customEvent(QEvent *event);

private slots:
  /** Called when the user selects the "Help" action on the toolbar. */
  void help();
  /** Called when the user selects the "Refresh" action on the toolbar */
  void refresh();
  /** Called when the user selects a circuit on the circuit list */
  void circuitSelected(Circuit circuit);
  /** Called when an IP has been resolved to geographic information. */
  void resolved(int id, QList<GeoIp> geoips);
  /** Called when the user selects a router in the list. */
  void routerSelected(RouterDescriptor router);
  /** Handles when we get connected to Tor network */
  void onAuthenticated();
  /** Handles when we get disconnected from Tor network */
  void onDisconnected();
  /** Resolves IP addresses in the resolve queue to geographic information. */
  void resolve();
  
private:
  /** Adds an IP address to the resolve queue and updates the queue timers. */
  void addToResolveQueue(QHostAddress ip, QString id);
  /** Loads a list of router descriptors from the list of IDs. */
  void loadDescriptors(QStringList ids);
  /** Loads a list of address mappings from Tor. */
  void loadAddressMap();
  /** Adds a router to our list of servers and retrieves geographic location
   * information for the server. */
  void addRouter(RouterDescriptor rd);
  /** Convert all hops in <b>circ</b>'s path to server identities.
  * <b>circ</b>'s status and circuit ID will be preserved. */
  Circuit circuitPathIDs(Circuit circ);
  /** Convert all hops in <b>circ</b>'s path to server names. <b>circ</b>'s
   * status and circuit ID will be preserved. */
  Circuit circuitPathNames(Circuit circ);

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
