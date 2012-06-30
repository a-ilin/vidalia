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
  : _bytesReceived(0), _bytesSent(0)
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

/** Formats a count of bytes appropriately as bytes, kilobytes,
 * megabytes, or gigabytes. */
static
QString formatBytes(quint64 bytes) {
  const int K = 1024, M = K * K, G = K * M;
  double d = bytes;
  /*if (bytes < 1 * K) return QString("%1 B").arg(bytes);*/
  if (bytes < 10 * K) return QString("%1 KB").arg(d / K, 0, 'f', 1);
  if (bytes < 1 * M) return QString("%1 KB").arg(d / K, 0, 'f', 0);
  if (bytes < 100 * M) return QString("%1 MB").arg(d / M, 0, 'f', 1);
  if (bytes < 1 * G) return QString("%1 MB").arg(d / M, 0, 'f', 0);
  return QString("%1 GB").arg(d / G, 0, 'f', 1);
}

/** Updates the traffic totals of this stream item. */
void
StreamItem::setTraffic(quint64 bytesReceived, quint64 bytesSent)
{
  _bytesReceived = bytesReceived;
  _bytesSent = bytesSent;

  QString str;
  if (bytesReceived | bytesSent) {
    str = QString("%1 down / %2 up").arg(
      formatBytes(bytesReceived),
      formatBytes(bytesSent));
  } else {
    str = QString();
  }

  setText(CircuitListWidget::TrafficColumn, str);
  setToolTip(CircuitListWidget::TrafficColumn, str);
}
