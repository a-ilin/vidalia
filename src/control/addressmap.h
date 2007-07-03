/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
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
 * \file addressmap.h
 * \version $Id: bandwidthevent.h 1563 2006-12-26 06:06:04Z edmanm $
 * \brief Stores a list of address mappings and their expiration times
 */

#ifndef _ADDRESSMAP_H
#define _ADDRESSMAP_H

#include <QHash>
#include <QDateTime>
#include <QPair>

/** Defines a type that pairs a mapping's target address with an expiration
 * time for that mapping. */
typedef QPair<QString, QDateTime> addr_map_entry_t;


class AddressMap : public QHash<QString, addr_map_entry_t>
{
public:
  /** Types of address mappings. */
  enum AddressMapType {
    AddressMapAll,    /**< All address mapping types. */
    AddressMapConfig, /**< Address mappings set in the torrc. */
    AddressMapCache,  /**< Address mappings cached by Tor. */
    AddressMapControl /**< Address mappings set by a controller. */
  };

  /** Constructor. Creates an empty table for storing address mappinsgs. */
  AddressMap()
   : QHash<QString, addr_map_entry_t>() {}

  /** Adds a new address mapping or updates an existing one for the address
   * specified by <b>from</b>. The mapping will remain valid until the date in
   * <b>expires</b>. */
  void add(QString from, QString to, QDateTime expires);
  /** Adds a new address mapping or updates an existing one based on fields
   * parsed from <b>mapping</b>. */
  void add(QString mapping);

  /** Returns true if the address map table contains a mapping for <b>addr</b>
   * that is not expired. */
  bool isMapped(QString addr) const;
  
  /** Returns the address to which <b>addr</b> is currently mapped. If there
   * is no mapping for <b>addr</b> (or the mapping is expired), then an
   * empty string is returned. */
  QString mappedTo(QString addr) const;

  /** Returns the reverse of this address map. */
  AddressMap reverse() const;

private:
  /** Returns true if <b>entry</b> is expired; false otherwise. */
  bool isExpired(addr_map_entry_t entry) const;
};

#endif

