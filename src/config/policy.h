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
 * \file policy.h
 * \version $Id$
 */

#ifndef _POLICY_H
#define _POLICY_H

#include <QString>
#include <QHostAddress>


class Policy
{
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

  /** Overloads the == operator. */
  bool operator==(const Policy &policy) const;

  /** Parses the given policy string. */
  void fromString(QString policy);
  /** Converts this policy to a format Tor understands. */
  QString toString();
  /** Converts a string action to an Action enum value. */
  static Action toAction(QString action);
  
  /** Returns the action taken when this policy matches an address. */
  QString action();
  /** Returns the host address (including mask, if set) for this policy. */
  QString address();
  /** Returns the port or port range for this policy. */
  QString ports();

private:
  Action _action; /**< The action to take for this policy. */
  QHostAddress _address; /**< Addresses to which this policy applies. */
  quint16 _fromPort;  /**< Start of a port range. */
  quint16 _toPort;  /**< End of a port range. */
  uchar _mask;  /**< Address mask. */
};

#endif

