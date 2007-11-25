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
 * \file circuitlistwidget.cpp
 * \version $Id$
 * \brief Collection of Tor circuits as CircuitItems
 */

#include <QPoint>
#include <QTimer>
#include <vidalia.h>

#include "circuitlistwidget.h"

#define IMG_CLOSE   ":/images/16x16/network-offline.png"
#define IMG_ZOOM    ":/images/16x16/zoom.png"

#define CLOSED_CIRCUIT_REMOVE_DELAY     3000
#define FAILED_CIRCUIT_REMOVE_DELAY     5000
#define CLOSED_STREAM_REMOVE_DELAY      3000
#define FAILED_STREAM_REMOVE_DELAY      4000


/** Default constructor. */
CircuitListWidget::CircuitListWidget(QWidget *parent)
: QTreeWidget(parent)
{
  /* Create and initialize columns */
  setHeaderLabels(QStringList() << tr("Connection") << tr("Status"));

  /* Find out when a circuit has been selected */
  connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          this, SLOT(onSelectionChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(customContextMenuRequested(QPoint)));

  /* Respond to the Delete key by closing whatever circuits or streams are
   * selected. */
  vApp->createShortcut(QKeySequence::Delete, this, this,
                       SLOT(closeSelectedConnections()));
}

/** Called when the user requests a context menu on a circuit or stream in the
 * list and displays a context menu appropriate for whichever type of item is
 * currently selected. */
void
CircuitListWidget::customContextMenuRequested(const QPoint &pos)
{
  QMenu menu(this);

  /* Find out which item was right-clicked */
  QTreeWidgetItem *item = itemAt(pos);
  if (!item)
    return;
    
  if (!item->parent()) {
    /* A circuit was selected */
    CircuitItem *circuitItem = dynamic_cast<CircuitItem *>(item);
    if (!circuitItem)
      return;

    /* Set up the circuit context menu */
    QAction *zoomAct  = new QAction(QIcon(IMG_ZOOM),
                                    tr("Zoom to Circuit"), this);
    QAction *closeAct = new QAction(QIcon(IMG_CLOSE),
                                    tr("Close Circuit (Del)"), this);
    zoomAct->setEnabled(circuitItem->circuit().status() == Circuit::Built);
    menu.addAction(zoomAct);
    menu.addSeparator();
    menu.addAction(closeAct);
      
    /* Display the context menu and find out which (if any) action was
     * selected */
    QAction* action = menu.exec(mapToGlobal(pos));
    if (action == closeAct)
      emit closeCircuit(circuitItem->id());
    else if (action == zoomAct)
      emit zoomToCircuit(circuitItem->id());
  } else {
    /* A stream was selected */
    StreamItem *streamItem = dynamic_cast<StreamItem *>(item);
    if (!streamItem)
      return;
 
    /* Set up the stream context menu */
    QAction *closeAct = new QAction(QIcon(IMG_CLOSE),
                                    tr("Close Stream (Del)"), this);
    menu.addAction(closeAct);

    /* Display the context menu and find out which (if any) action was
     * selected */
    QAction* action = menu.exec(mapToGlobal(pos));
    if (action == closeAct)
      emit closeStream(streamItem->id());
  }
}

/** Closes all selected circuits or streams. */
void
CircuitListWidget::closeSelectedConnections()
{
  QList<QTreeWidgetItem *> items = selectedItems();
  foreach (QTreeWidgetItem *item, items) {
    if (!item->parent()) {
      CircuitItem *circuitItem = dynamic_cast<CircuitItem *>(item);
      if (circuitItem)
        emit closeCircuit(circuitItem->id());
    } else {
      StreamItem *streamItem = dynamic_cast<StreamItem *>(item);
      if (streamItem)
        emit closeStream(streamItem->id());
    }
  }
}

/** Adds a circuit to the list. If the circuit already exists in the list, the
 * status and path will be updated. <b>displayedPath</b> contains a
 * description of the circuit's path suitable for humans to read. */
void
CircuitListWidget::addCircuit(Circuit circuit, QString displayedPath)
{
  /* Check to see if the circuit already exists in the tree */
  CircuitItem *item = findCircuitItem(circuit.id());
  
  if (!item) {
    /* Add the new circuit */
    item = new CircuitItem(circuit, displayedPath);
    addTopLevelItem(item);
  } else {
    /* Circuit already exists, so update its status and path */
    item->update(circuit, displayedPath);
  }

  /* If the circuit is closed or dead, schedule it for removal */
  Circuit::Status status = circuit.status();
  if (status == Circuit::Closed) {
    scheduleCircuitRemoval(item, CLOSED_CIRCUIT_REMOVE_DELAY);
  } else if (status == Circuit::Failed) {
    scheduleCircuitRemoval(item, FAILED_CIRCUIT_REMOVE_DELAY);
  }
}

/** Adds a stream to the list. If the stream already exists in the list, the
 * status and path will be updated. */
void
CircuitListWidget::addStream(Stream stream)
{
  /* Check to see if the stream already exists in the tree */
  StreamItem *item = findStreamItem(stream.id());

  if (!item) {
    CircuitItem *circuit = findCircuitItem(stream.circuitId());
    /* New stream, so try to find its circuit and add it */
    if (circuit) {
      circuit->addStream(new StreamItem(stream));
      expandItem(circuit);
    }
  } else {
    /* Stream already exists, so just update its status */
    item->update(stream);

    /* If the stream is closed or dead, schedule it for removal */
    Stream::Status status = stream.status();
    if (status == Stream::Closed) {
      scheduleStreamRemoval(item, CLOSED_STREAM_REMOVE_DELAY);
    } else if (status == Stream::Failed) {
      scheduleStreamRemoval(item, FAILED_STREAM_REMOVE_DELAY);
    }
  }
}

/** Schedules the given circuit to be removed after the specified timeout. */
void
CircuitListWidget::scheduleCircuitRemoval(CircuitItem *circuit, int delay)
{
  if (!_circuitRemovalList.contains(circuit)) {
    _circuitRemovalList << circuit;
    QTimer::singleShot(delay, this, SLOT(removeCircuit()));
  } 
}

/** Schedules the given stream to be removed after the specified timeout. */
void
CircuitListWidget::scheduleStreamRemoval(StreamItem *stream, int delay)
{
  if (!_streamRemovalList.contains(stream)) {
    _streamRemovalList << stream;
    QTimer::singleShot(delay, this, SLOT(removeStream()));
  } 
}

/** Removes the first circuit scheduled to be removed. */
void
CircuitListWidget::removeCircuit()
{
  if (!_circuitRemovalList.isEmpty()) {
    CircuitItem *circuitItem = _circuitRemovalList.takeFirst();
    Circuit circuit = circuitItem->circuit();
    removeCircuit(circuitItem);
    emit circuitRemoved(circuit.id());
  }
}

/** Removes the given circuit item and all streams on that circuit. */
void
CircuitListWidget::removeCircuit(CircuitItem *circuit)
{
  if (circuit) {
    /* Remove all streams (if any) on this circuit. */
    QList<StreamItem *> streams = circuit->streams();
    foreach (StreamItem *stream, streams) {
      /* Check if this stream was scheduled for removal already */
      if (_streamRemovalList.contains(stream)) {
        /* If this stream was already scheduled for removal, replace its pointer
         * with 0, so it doesn't get removed twice. */
        int index = _streamRemovalList.indexOf(stream);
        _streamRemovalList.replace(index, (StreamItem *)0);
      }
      
      /* Remove the stream item from the circuit */
      circuit->removeStream(stream);
    }
    /* Remove the circuit item itself */
    delete takeTopLevelItem(indexOfTopLevelItem(circuit));
  }
}

/** Removes the first stream scheduled to be removed. */
void
CircuitListWidget::removeStream()
{
  if (!_streamRemovalList.isEmpty()) {
    StreamItem *stream = _streamRemovalList.takeFirst();
    removeStream(stream);
  }
}

/** Removes the given stream item. */
void
CircuitListWidget::removeStream(StreamItem *stream)
{
  if (stream) {
    /* Try to get the stream's parent (a circuit item) */ 
    CircuitItem *circuit = (CircuitItem *)stream->parent();
    if (circuit) {
      /* Remove the stream from the circuit and delete the item */
      circuit->removeStream(stream);
    } else {
      /* It isn't on a circuit, so just delete the stream */
      delete stream;
    }
  }
}

/** Clears all circuits and streams from the list. */
void
CircuitListWidget::clearCircuits()
{
  QTreeWidget::clear();
  _circuitRemovalList.clear();
  _streamRemovalList.clear();
}

/** Finds the circuit with the given ID and returns a pointer to that
 * circuit's item in the list. */
CircuitItem*
CircuitListWidget::findCircuitItem(quint64 circid)
{
  int numCircs = topLevelItemCount();
  for (int i = 0; i < numCircs; i++) {
    CircuitItem *circuit = (CircuitItem *)topLevelItem(i);
    if (circid == circuit->id()) {
      return circuit;
    }
  }
  return 0;
}

/** Finds the stream with the given ID and returns a pointer to that stream's
 * item in the list. */
StreamItem*
CircuitListWidget::findStreamItem(quint64 streamid)
{
  int numCircs = topLevelItemCount();
  int numStreams;
  
  for (int i = 0; i < numCircs; i++) {
    CircuitItem *circuit = (CircuitItem *)topLevelItem(i);
    numStreams = circuit->childCount();
  
    for (int j = 0; j < numStreams; j++) {
      StreamItem *stream = (StreamItem *)circuit->child(j);
      if (streamid == stream->id()) {
        return stream;
      }
    }
  }
  return 0;
}

/** Called when the current item selection has changed. */
void
CircuitListWidget::onSelectionChanged(QTreeWidgetItem *cur, 
                                      QTreeWidgetItem *prev)
{
  Q_UNUSED(prev);

  if (cur) {
    Circuit circuit;
    
    if (!cur->parent()) {
      /* User selected a CircuitItem, so just grab the Circuit */
      circuit = ((CircuitItem *)cur)->circuit();
    } else {
      /* User selected a StreamItem, so get its parent and then the Circuit */
      CircuitItem *circItem = (CircuitItem *)cur->parent();
      circuit = circItem->circuit();
    }

    /* If this circuit has a path, then emit it so we can highlight it */
    if (circuit.length() > 0) {
      emit circuitSelected(circuit);
    }
  }
}

/** Returns a list of circuits currently in the widget. */
QList<Circuit>
CircuitListWidget::circuits()
{
  int numCircs = topLevelItemCount();
  QList<Circuit> circs;
  
  for (int i = 0; i < numCircs; i++) {
    CircuitItem *circ = (CircuitItem *)topLevelItem(i);
    circs << circ->circuit();
  }
  return circs;
}

