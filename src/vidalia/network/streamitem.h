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
 * \file streamitem.h
 * \version $Id$
 * \brief Item representing a stream through Tor and its status
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

  /** Updates the status of this stream item. */
  void update(const Stream &stream);
  /** Returns the ID of the stream associated with this tree item. */
  quint64 id() const { return _id; }
  
private:
  quint64 _id; /**< ID for this stream. */
};

#endif

