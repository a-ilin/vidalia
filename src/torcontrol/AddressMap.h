/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file AddressMap.h
** \brief Stores a list of address mappings and their expiration times
*/

#ifndef _ADDRESSMAP_H
#define _ADDRESSMAP_H

#include <QHash>
#include <QDateTime>
#include <QPair>

/** Defines a type that pairs a mapping's target address with an expiration
 * time for that mapping. */
typedef QPair<QString, QDateTime> AddressMapEntry;


class AddressMap : public QHash<QString, AddressMapEntry>
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
   : QHash<QString, AddressMapEntry>() {}

  /** Adds a new address mapping or updates an existing one for the address
   * specified by <b>from</b>. The mapping will remain valid until the date in
   * <b>expires</b>. */
  void add(const QString &from, const QString &to, const QDateTime &expires);
  /** Adds a new address mapping or updates an existing one based on fields
   * parsed from <b>mapping</b>. */
  void add(const QString &mapping);

  /** Returns true if the address map table contains a mapping for <b>addr</b>
   * that is not expired. */
  bool isMapped(const QString &addr) const;

  /** Returns the address to which <b>addr</b> is currently mapped. If there
   * is no mapping for <b>addr</b> (or the mapping is expired), then an
   * empty string is returned. */
  QString mappedTo(const QString &addr) const;

  /** Returns the reverse of this address map. */
  AddressMap reverse() const;

private:
  /** Returns true if <b>entry</b> is expired; false otherwise. */
  bool isExpired(const AddressMapEntry &entry) const;
};

#endif

