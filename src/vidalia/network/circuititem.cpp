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
** \file circuititem.cpp
** \version $Id$
** \brief Item representing a Tor circuit and its status
*/

#include "circuitlistwidget.h"
#include "circuititem.h"


/** Constructor */
CircuitItem::CircuitItem(Circuit circuit, QString displayedPath)
{
  /* Update the displayed text */
  update(circuit, displayedPath);
}

/** Updates the status and path of this circuit item. */
void
CircuitItem::update(Circuit circuit, QString displayedPath)
{
  /* Save the Circuit object */
  _circuit = circuit;
  
  /* Use a semi-meaningful value if the path is empty */
  if (displayedPath.isEmpty()) {
    displayedPath = tr("<Path Empty>");
  }

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
CircuitItem::streams()
{
  QList<StreamItem *> streams;
  int n = childCount();
  for (int i = 0; i < n; i++) {
    streams << (StreamItem *)child(i);
  }
  return streams;
}

