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
** \file StreamItem.cpp
** \brief Item representing a stream through Tor and its status
*/

#include "StreamItem.h"
#include "CircuitListWidget.h"


/** Constructor */
StreamItem::StreamItem(const Stream &stream)
{
  /* Set the stream's displayed target information */
  setText(CircuitListWidget::ConnectionColumn, stream.target());
  setToolTip(CircuitListWidget::ConnectionColumn, stream.target());

  /* Update the status and target */
  update(stream);
}

/** Updates the status of this stream item. */
void
StreamItem::update(const Stream &stream)
{
  _stream = stream;
  setText(CircuitListWidget::StatusColumn, stream.statusString());
  setToolTip(CircuitListWidget::StatusColumn, stream.statusString());
}

