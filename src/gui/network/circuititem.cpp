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
 * \file circuititem.cpp
 * \version $Id$
 * \brief Item representing a Tor circuit and its status
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

