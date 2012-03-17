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
** \file CircuitItem.h
** \brief List item representing a Tor circuit and its status
*/

#ifndef _CIRCUITITEM_H
#define _CIRCUITITEM_H

#include "StreamItem.h"

#include "Circuit.h"
#include "Stream.h"

#include <QApplication>
#include <QTreeWidgetItem>


class CircuitItem : public QTreeWidgetItem
{
  Q_DECLARE_TR_FUNCTIONS(CircuitItem)

public:
  /** Default constructor */
  CircuitItem(const Circuit &circuit);

  /** Adds a stream to this circuit item */
  void addStream(StreamItem *stream);
  /** Removes the stream item from the list and frees its memory. */
  void removeStream(StreamItem *stream);
  /** Updates the status of this circuit item using the given circuit. */
  void update(const Circuit &circuit);
  /** Returns the ID for this circuit. */
  CircuitId id() const { return _circuit.id(); }
  /** Returns the Circuit object for this item. */
  Circuit circuit() const { return _circuit; }
  /** Returns a list of all stream items on this circuit. */
  QList<StreamItem *> streams() const;

private:
  Circuit _circuit; /**< Circuit associated with this item. */
};

#endif

