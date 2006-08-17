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
 * \version $Id: netviewer.h 699 2006-04-15 03:12:22Z hipplej $
 */

#ifndef _NETVIEWER_H
#define _NETVIEWER_H

#include <QMainWindow>
#include <QStringList>
#include <QEvent>
#include <QTimer>
#include <control/torcontrol.h>
#include <util/geoip/geoipresolver.h>
#include <gui/common/vidaliawindow.h>

#include "tormapwidget.h"
#include "ui_netviewer.h"


class NetViewer : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Default constructor */
  NetViewer(QWidget* parent = 0);

public slots:
  /** Loads a list of current circuits and streams. */
  void loadConnections();
  /** Adds a circuit to the list and the map */
  void addCircuit(Circuit circuit);
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
  void gotConnected();
  /** Handles when we get disconnected from Tor network */
  void gotDisconnected();

private:
  /** Loads a list of router descriptors from the list of IDs. */
  void loadDescriptors(QStringList ids);

  /** TorControl object used to talk to Tor. */
  TorControl* _torControl;
  /** Timer that fires once an hour to update the router list. */
  QTimer* _timer;
  /** TorMapWidget that displays the map. */
  TorMapWidget* _map;
  /** GeoIpResolver used to geolocate routers by IP address. */
  GeoIpResolver _geoip;

  /** Qt Designer generated object **/
  Ui::NetViewer ui;
};

#endif

