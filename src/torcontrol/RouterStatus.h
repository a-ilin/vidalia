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
** \file RouterStatus.h
** \brief Parses a blob of router status text from Tor
*/

#ifndef _ROUTERSTATUS_H
#define _ROUTERSTATUS_H

#include <QFlags>
#include <QStringList>
#include <QHostAddress>
#include <QDateTime>


class RouterStatus
{
public:
  /** Possible router status flags. */
  enum Flag {
    Unknown      = 0x0000,
    Authority    = 0x0001,
    BadExit      = 0x0002,
    BadDirectory = 0x0004,
    Exit         = 0x0008,
    Fast         = 0x0010,
    Guard        = 0x0020,
    HSDir        = 0x0040,
    Named        = 0x0080,
    Stable       = 0x0100,
    Running      = 0x0200,
    Valid        = 0x0400,
    V2Dir        = 0x0800,
    V3Dir        = 0x1000
  };
  Q_DECLARE_FLAGS(Flags, Flag)

  /** Constructor. */
  RouterStatus(const QStringList &status);

  /** Returns the router's hexadecimal-encoded router identity key digest. */
  QString id() const { return _id; }
  /** Returns the router's nickname. */
  QString name() const { return _name; }
  /** Returns the hexadecimal-encoded digest of the router's most recent
   * descriptor. */
  QString descriptorDigest() const { return _digest; }
  /** Returns the router's most recent IP address. */
  QHostAddress ipAddress() const { return _ipAddress; }
  /** Returns the publication time of the router's most recent descriptor. */
  QDateTime published() const { return _published; }
  /** Returns the router's OR port number. */
  quint16 orPort() const { return _orPort; }
  /** Returns the router's directory port number. */
  quint16 dirPort() const { return _dirPort; }

  /** Returns an OR-ed field of the router's current status flags. */
  Flags flags() const { return _flags; }
  /** Returns true if this router is currently listed as Running. */
  bool isRunning() const { return (flags() & Running); }
  /** Returns the consensus bandwidth */
  quint64 bandwidth() const { return _bandwidth; }

  /** Returns true if this router status object is valid. This method should
   * be called to verify that the QStringList given in this object's
   * constructor contained properly formatted router status lines. */
  bool isValid() const { return _valid; }

private:
  /** Returns a Flags enum value for the given router status <b>flag</b>. If
   * <b>flag</b> is not recognized, then <i>Unknown</i> is returned. */
  Flag flagValue(const QString &flag);

  bool _valid;   /**< True if this object is a valid RouterStatus. */
  QString _name; /**< Router nickname. */
  QString _id;   /**< Hexadecimal-encoded router identity digest. */
  QString _digest; /**< Hexadecimal-encoded hash of the router's most recent
                        descriptor. */
  QDateTime _published; /**< The publication time of the router's most recent
                             descriptor. */
  QHostAddress _ipAddress;  /**< Current IP address. */
  quint16 _orPort;  /**< Current OR port. */
  quint16 _dirPort; /**< Current directory port. */
  Flags _flags;     /**< OR-ed field of the router's current status flags. */
  quint64 _bandwidth; /**< Consensus bandwidth */
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RouterStatus::Flags)

/** A collection of RouterStatus objects. */
typedef QList<RouterStatus> NetworkStatus;

#endif

