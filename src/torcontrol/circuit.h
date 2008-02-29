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
** \file circuit.h
** \version $Id$
** \brief Object representing a Tor circuit
*/

#ifndef _CIRCUIT_H
#define _CIRCUIT_H

#include <QCoreApplication>
#include <QStringList>
#include <QString>


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
  /** Constructor */
  Circuit(quint64 circId, Status status, QString path);
  /** Constructor */
  Circuit(quint64 circId, Status status, QStringList hops);

  /** Parses the given string (in Tor's control protocol format) */
  static Circuit fromString(QString circuit);
  /** Converts a string description of a circuit's status to an enum value */
  static Status toStatus(QString strStatus);
  
  /** Returns true if all fields in this Circuit are empty. */
  bool isEmpty() const;
  
  /** Returns the ID for this circuit */
  quint64 id() const { return _circId; }
  /** Returns the status of this circuit */
  Status status() const { return _status; }
  /** Returns a string representation of the status of this circuit. */
  QString statusString() const;
  /** Returns the path chosen for this circuit */
  QString path() const { return _path; }
  /** Returns the length of the circuit's path. */
  uint length() const { return hops().size(); }
  /** Returns a list of hops on the path. */
  QStringList hops() const { return _path.isEmpty() ? QStringList() 
                                                    : _path.split(","); }

private:
  quint64 _circId; /**< Circuit ID. */
  Status _status;  /**< Circuit status. */
  QString _path;   /**< Circuit path. */
};

#endif

