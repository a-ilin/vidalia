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
** \file streamitem.h
** \version $Id$
** \brief Item representing a stream through Tor and its status
*/

#ifndef _STREAMITEM_H
#define _STREAMITEM_H

#include <QTreeWidgetItem>
#include <stream.h>


class StreamItem : public QTreeWidgetItem
{
public:
  /** Constructor */
  StreamItem(const Stream &stream);

  Stream stream() const { return _stream; }
  /** Updates the status of this stream item. */
  void update(const Stream &stream);
  /** Returns the ID of the stream associated with this tree item. */
  StreamId id() const { return _stream.id(); }
  
private:
  Stream _stream;
};

#endif

