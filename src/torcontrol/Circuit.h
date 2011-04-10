/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If 
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to
**  the terms described in the LICENSE file.
*/

/*
** \file Circuit.h
** \brief Object representing a Tor circuit
*/

#ifndef _CIRCUIT_H
#define _CIRCUIT_H

#include <QCoreApplication>
#include <QStringList>
#include <QMetaType>

/** Circuit IDs contains 1-16 alphanumeric ASCII characters. */
typedef QString CircuitId;


class Circuit
{
  Q_DECLARE_TR_FUNCTIONS(Circuit)

public:
  /** Circuit status events */
  enum Status {
    Unknown,   /**< Unknown circuit status */
    Launched,  /**< Circuit ID assigned to new circuit */
    Built,     /**< All hops finished */
    Extended,  /**< Circuit extended by one hop */
    Failed,    /**< Circuit closed (was not built) */
    Closed     /**< Circuit closed (was built) */
  };

  /** Default constructor. */
  Circuit();
  /** Constructor. */  
  Circuit(const CircuitId &circuit);
  
  /** Returns true if this circuit is valid. */
  bool isValid() const { return _isValid; }
  
  /** Returns the ID for this circuit */
  CircuitId id() const { return _circId; }
  /** Returns the status of this circuit */
  Status status() const { return _status; }
  /** Returns a string representation of the status of this circuit. */
  QString statusString() const;
  /** Returns the length of the circuit's path. */
  uint length() const { return _ids.size(); }
  /** Returns the circuit's path as an ordered list of router nicknames. */
  QStringList routerNames() const { return _names; }
  /** Returns the circuit's path as an ordered list of router fingerprints. */
  QStringList routerIDs() const { return _ids; }

  /** Converts a string description of a circuit's status to an enum value */
  static Status toStatus(const QString &strStatus);

  /** Returns true iff <b>circId</b> consists of only between 1 and 16
   * (inclusive) ASCII-encoded letters and numbers. */
  static bool isValidCircuitId(const CircuitId &circId);

private:
  CircuitId _circId; /**< Circuit ID. */
  Status _status;  /**< Circuit status. */
  QStringList _names;  /**< Nicknames of the routers in the circuit. */
  QStringList _ids;    /**< IDs of the routers in the circuit. */
  bool _isValid;
};

Q_DECLARE_METATYPE(Circuit);

/** A collection of circuits. */
typedef QList<Circuit> CircuitList;

#endif

