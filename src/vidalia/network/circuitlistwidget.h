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
 * \file circuitlistwidget.h
 * \version $Id$
 * \brief Collection of Tor circuits as CircuitItems
 */

#ifndef _CIRCUITLISTWIDGET_H
#define _CIRCUITLISTWIDGET_H

#include <QTreeWidget>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include "circuititem.h"
#include "streamitem.h"


class CircuitListWidget : public QTreeWidget
{
  Q_OBJECT
  
public:
  /** Circuit list columns. */
  enum Columns {
    ConnectionColumn = 0, /**< Column for either the circuit or stream */
    StatusColumn = 1      /**< Status of the connection. */
  };
  
  /** Default constructor */
  CircuitListWidget(QWidget *parent = 0);

  /** Adds a circuit to the list. If the circuit already exists in the list,
   * the status and path will be updated. */
  void addCircuit(Circuit circuit, QString displayedPath);
  /** Adds a stream to the list. If the stream already exists in the list, the
   * status and path will be updated. */
  void addStream(Stream stream);
  /** Returns a list of circuits currently in the widget. */
  QList<Circuit> circuits();

signals:
  /** Emitted when a circuit item is selected. */
  void circuitSelected(Circuit circuit);
  /** Emitted when a circuit is removed from the list. */
  void circuitRemoved(quint64 circid);
  /** Emitted when the user selects a circuit to be closed. */
  void closeCircuit(quint64 circid);
  /** Emitted when the user selects a stream to be closed. */
  void closeStream(quint64 streamid);
  /** Emitted when the user selects a circuit to zoom to. */
  void zoomToCircuit(quint64 circid);
  
public slots:
  /** Clears all circuits and streams from the list. */
  void clearCircuits();

private slots:
  /** Removes the first circuit scheduled to be removed.*/
  void removeCircuit(); 
  /** Removes the first stream scheduled to be removed. */
  void removeStream();
  /** Called when the current item selectio has changed. */
  void onSelectionChanged(QTreeWidgetItem *cur, QTreeWidgetItem *prev);
  /** Called when the user requests a context menu on a circuit or stream in
   * the list and displays a context menu appropriate for whichever type of
   * item is currently selected. */
  void customContextMenuRequested(const QPoint &pos);
  /** Closes all selected circuits or streams. */
  void closeSelectedConnections();

private:
  /** Removes the given circuit item and all streams on that circuit. */
  void removeCircuit(CircuitItem *circuit);
  /** Removes the given stream item. */
  void removeStream(StreamItem *stream);
  /** Finds the circuit with the given ID. */
  CircuitItem* findCircuitItem(quint64 circid);
  /** Finds the stream with the given ID. */
  StreamItem* findStreamItem(quint64 streamid);
  /** Schedules the given circuit item to be removed after the given timeout. */
  void scheduleCircuitRemoval(CircuitItem *circuit, int delay);
  /** Schedules a stream to be removed after the given timeout. */
  void scheduleStreamRemoval(StreamItem *stream, int delay);

  /** List of circuit items to be removed. */
  QList<CircuitItem *> _circuitRemovalList;
  /** List of stream items to be removed. */
  QList<StreamItem *> _streamRemovalList;
};

#endif

