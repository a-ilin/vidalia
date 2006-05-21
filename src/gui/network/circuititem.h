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
 * \file circuititem.h
 * \version $Id$
 */

#ifndef _CIRCUITITEM_H
#define _CIRCUITITEM_H

#include <QApplication>
#include <QTreeWidgetItem>
#include <control/circuit.h>
#include <control/stream.h>

#include "streamitem.h"


class CircuitItem : public QTreeWidgetItem
{
  Q_DECLARE_TR_FUNCTIONS(CircuitItem)

public:
  /** Default constructor */
  CircuitItem(Circuit circuit);
 
  /** Adds a stream to this circuit item */
  void addStream(StreamItem *stream);
  /** Updates the status of this circuit item using the given circuit. */
  void update(Circuit circuit);
  /** Returns the ID for this circuit. */
  quint64 id() { return _id; }
  
private:
  quint64 _id; /**< Circuit ID associated with this item. */
};

#endif

