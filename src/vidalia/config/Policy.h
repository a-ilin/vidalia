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
** \file Policy.h
** \brief Exit policy parsing
*/

#ifndef _POLICY_H
#define _POLICY_H

#include <QCoreApplication>
#include <QString>
#include <QHostAddress>


class Policy
{
  Q_DECLARE_TR_FUNCTIONS(Policy)

public:
  /** A set of possible actions for a policy */
  enum Action {
    Accept, /**< Connections matching this policy will be accepted. */
    Reject  /**< Connections matching this policy will be rejected. */
  };
  /** Special rule types. */
  enum SpecialPolicy {
    AcceptAll, /**< Accepts all connections. Equivalent to "accept *:*". */
    RejectAll  /**< Rejects all connections. Equivalent to "reject *:*". */
  };

  /** Default constructor. Creates an AcceptAll policy. */
  Policy();
  /** Parses the given policy, represented as a string. */
  Policy(QString policy);
  /** Parses the given portions of a policy string. */
  Policy(QString action, QString address, QString ports);
  /** Creates a policy of the given special type. */
  Policy(SpecialPolicy policy);
  /** Creates a policy using the specified information. */
  Policy(Action action, QHostAddress addr, uchar mask,
         quint16 fromPort, quint16 toPort = 0);

  /** Returns true if this policy matches <b>policy</b>. */
  bool matches(const Policy &policy) const;
  /** Returns true if this policy is identical to <b>policy</b>. */
  bool operator==(const Policy &policy) const;

  /** Parses the given policy string. */
  void fromString(QString policy);
  /** Converts this policy to a format Tor understands. */
  QString toString() const;
  /** Converts a string action to an Action enum value. */
  static Action toAction(QString action);

  /** Returns the action taken when this policy matches an address. */
  QString action() const;
  /** Returns the host address (including mask, if set) for this policy. */
  QString address() const;
  /** Returns the port or port range for this policy. */
  QString ports() const;

private:
  Action _action; /**< The action to take for this policy. */
  QHostAddress _address; /**< Addresses to which this policy applies. */
  quint16 _fromPort;  /**< Start of a port range. */
  quint16 _toPort;  /**< End of a port range. */
  uchar _mask;  /**< Address mask. */
};

#endif

