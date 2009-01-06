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
** \file addressmap.cpp
** \version $Id: bandwidthevent.h 1563 2006-12-26 06:06:04Z edmanm $
** \brief Stores a list of address mappings and their expiration times
*/

#include <QStringList>
#include "tcglobal.h"
#include "addressmap.h"

/** Format of expiry times in address map events. */
#define DATE_FMT "\"yyyy-MM-dd HH:mm:ss\""


/** Adds a new address mapping from the address <b>from</b> to the address
 * <b>to</b>, that expires at <b>expires</b>. */
void
AddressMap::add(QString from, QString to, QDateTime expires)
{
  tc::debug("New address mapping: %1 -> %2 (expires %3)").arg(from)
                                                          .arg(to)
                          .arg(expires.isValid() ? expires.toString(DATE_FMT)
                                                 : "never");
  insert(from, addr_map_entry_t(to, expires));
}

/** Adds a new address mapping by parsing the fields in <b>mapping</b>, which
 * should be formatted as follows: 
 *
 *   Address SP Address SP Expiry
 *   Expiry = DQUOTE ISOTime DQUOTE / "NEVER"
 */
void
AddressMap::add(QString mapping)
{
  QStringList parts = mapping.split(" ");
  if (parts.size() >= 2) {
    QDateTime expires;
    if (parts.size() >= 4 && parts.at(2) != "NEVER") {
      expires = QDateTime::fromString(parts.at(2) + " " + parts.at(3),
                                      DATE_FMT);

      /* Tor gives us the expiry time in UTC, but we will do subsequent
       * comparisons based on local time. So do the proper conversion now. */
      expires.setTimeSpec(Qt::UTC);
      expires = expires.toLocalTime();
    }
    add(parts.at(0), parts.at(1), expires);
  }
}

/** Returns true if <b>entry</b> is expired; false otherwise. */
bool
AddressMap::isExpired(addr_map_entry_t entry) const
{
  if (entry.second.isValid())
    return (entry.second < QDateTime::currentDateTime());
  return false; /* No expiry time == valid forever */
}

/** Returns true if there exists a mapping for <b>addr</b> and that mapping is
 * not expired. */
bool
AddressMap::isMapped(QString addr) const
{
  return (contains(addr) && !isExpired(value(addr)));
}

/** Returns the address to which <b>addr</b> is currently mapped. If there is
 * no mapping for <b>addr</b> (or the mapping is expired), then an empty
 * string is returned. */
QString
AddressMap::mappedTo(QString addr) const
{
  addr_map_entry_t entry = value(addr);
  return (isExpired(entry) ? QString() : entry.first);
}

/** Returns the reverse of this address map by swapping each address in the
 * address map with its mapped address. The expiration times are unaltered. */
AddressMap
AddressMap::reverse() const
{
  AddressMap reverseMap;
  foreach (QString from, keys()) {
    /* Flip the "from" and the "to" addresses and retain the expiry time. */
    addr_map_entry_t entry = value(from);
    reverseMap.add(entry.first, from, entry.second);
  }
  return reverseMap;
}

