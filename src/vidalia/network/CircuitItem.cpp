/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file CircuitItem.cpp
** \brief Item representing a Tor circuit and its status
*/

#include "CircuitItem.h"
#include "CircuitListWidget.h"


/** Constructor */
CircuitItem::CircuitItem(const Circuit &circuit)
{
  /* Update the displayed text */
  update(circuit);
}

/** Updates the status and path of this circuit item. */
void
CircuitItem::update(const Circuit &circuit)
{
  QString displayedPath;

  /* Save the Circuit object */
  _circuit = circuit;

  /* Use a semi-meaningful value if the path is empty */
  displayedPath = circuit.length() > 0 ? circuit.routerNames().join(",")
                                       : tr("<Path Empty>");

  /* Update the column fields */
  setText(CircuitListWidget::ConnectionColumn, displayedPath);
  setToolTip(CircuitListWidget::ConnectionColumn, displayedPath);
  setText(CircuitListWidget::StatusColumn, circuit.statusString());
  setToolTip(CircuitListWidget::StatusColumn, circuit.statusString());
}

/** Adds a stream as a child of this circuit. */
void
CircuitItem::addStream(StreamItem *stream)
{
  addChild(stream);
}

/** Removes the stream item from this circuit and frees its memory */
void
CircuitItem::removeStream(StreamItem *stream)
{
  int index = indexOfChild(stream);
  if (index > -1) {
    delete takeChild(index);
  }
}

/** Returns a list of all stream items on this circuit. */
QList<StreamItem *>
CircuitItem::streams() const
{
  QList<StreamItem *> streams;
  int n = childCount();
  for (int i = 0; i < n; i++) {
    streams << (StreamItem *)child(i);
  }
  return streams;
}

