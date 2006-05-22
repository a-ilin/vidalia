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
 */

#include <QTimer>

#include "circuitlistwidget.h"

#define CLOSED_CIRCUIT_REMOVE_DELAY     3000
#define FAILED_CIRCUIT_REMOVE_DELAY     5000
#define CLOSED_STREAM_REMOVE_DELAY      3000
#define FAILED_STREAM_REMOVE_DELAY      4000


/** Default constructor. */
CircuitListWidget::CircuitListWidget(QWidget *parent)
: QTreeWidget(parent)
{
  /* Find out when a circuit has been selected */
  connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          this, SLOT(onSelectionChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

/** Adds a circuit to the list. If the circuit already exists in the list, the
 * status and path will be updated. */
void
CircuitListWidget::addCircuit(Circuit circuit)
{
  /* Check to see if the circuit already exists in the tree */
  CircuitItem *item = findCircuitItem(circuit.id());
  
  if (!item) {
    /* Add the new circuit */
    addTopLevelItem(new CircuitItem(circuit));
  } else {
    /* Circuit already exists, so update its status and path */
    item->update(circuit);

    /* If the circuit is closed or dead, schedule it for removal */
    Circuit::Status status = circuit.status();
    if (status == Circuit::Closed) {
      scheduleCircuitRemoval(item, CLOSED_CIRCUIT_REMOVE_DELAY);
    } else if (status == Circuit::Failed) {
      scheduleCircuitRemoval(item, FAILED_CIRCUIT_REMOVE_DELAY);
    }
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
    CircuitItem *circuit = _circuitRemovalList.takeFirst();
    removeCircuit(circuit);
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
CircuitListWidget::clear()
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

  if (cur && !cur->parent()) {
    Circuit circuit = ((CircuitItem *)cur)->circuit();
    if (circuit.length() > 0) {
      emit circuitSelected(circuit);
    }
  }
}

