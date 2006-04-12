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
 * \file policy.cpp
 * \version $Id$
 */

#include <QStringList>

#include "policy.h"


/** Default constructor. Creates an AcceptAll policy. */
Policy::Policy()
{
  _action   = Accept;
  _address  = QHostAddress::Any;
  _fromPort = _toPort = 0;
  _mask = 0;
}

/** Constructor. Creates a new Policy object from the given string. */
Policy::Policy(QString policy)
{
  /* Set the defaults */
  _action   = Accept;
  _address  = QHostAddress::Any;
  _fromPort = _toPort = 0;
  _mask = 0;
  
  /* Parse the given string to override the defaults. */
  fromString(policy);
}

/** Constructor. Creates a new Policy object from the string parts. */
Policy::Policy(QString action, QString address, QString ports)
{
  /* Set the defaults */
  _action   = Accept;
  _address  = QHostAddress::Any;
  _fromPort = _toPort = 0;
  _mask = 0;
  
  fromString(action + " " + address + ":" + ports);
}

/** Constructor. Creates a new Policy object depending on the specified
 * special policy type. */
Policy::Policy(SpecialPolicy policy)
{
  _action   = (policy == AcceptAll ? Accept : Reject);
  _address  = QHostAddress::Any;
  _fromPort = _toPort = 0;
  _mask = 0;
}

/** Constructor. Creates a new policy object based on the given rules. */
Policy::Policy(Action action, QHostAddress addr, uchar mask, 
               quint16 fromPort, quint16 toPort)
{
  _action   = action;
  _address  = addr;
  _fromPort = fromPort;
  _toPort   = toPort;
  _mask = mask;
}

/** Overloads the == operator. */
bool
Policy::operator==(const Policy &policy) const
{
  return (this->_action   == policy._action &&
          this->_address  == policy._address &&
          this->_mask     == policy._mask &&
          this->_fromPort == policy._fromPort &&
          this->_toPort   == policy._toPort);
}

/** Parses the given exit policy string. */
void
Policy::fromString(QString policy)
{
  /* Separate the action and the address/mask/port info */
  QStringList ruleParts = policy.split(" ");
  _action = (ruleParts.at(0).toLower() == "reject" ? Reject : Accept);
  
  /* If some address/mask/port stuff was specified, parse it. */
  if (ruleParts.size() > 1) {
    QStringList addrParts = ruleParts.at(1).split(":");

    /* Parse the address and mask (if specified) */
    QString addr = addrParts.at(0);
    _address.setAddress(addr.mid(0, addr.indexOf("/")));
    if (addr.contains("/")) {
      _mask = addr.mid(addr.indexOf("/")+1).toUInt();
    }

    /* Parse the specified port range (if specified) */
    if (addrParts.size() > 1) {
      QString ports = addrParts.at(1);
      _fromPort = ports.mid(0, ports.indexOf("-")).toUInt();
      if (ports.contains("-")) {
        _toPort = ports.mid(ports.indexOf("-")+1).toUInt();
      }
    }
  }
}

/** Converts this policy to a form Tor understands. The format is:
 *         "accept|reject ADDR[/MASK][:PORT]"
 *
 * PORT can be a single port number, an interval of ports "FROM_PORT-TO_PORT",
 * or "*". If PORT is omitted, that means "*"
 */
QString
Policy::toString()
{
  return action() + " " + address() + ":" + ports();
}

/** Converts the given action to a string. */
Policy::Action
Policy::toAction(QString action)
{
  if (action.toLower() == "accept") {
    return Accept;
  }
  return Reject;
}

/** Returns the action associated with this policy. */
QString
Policy::action()
{
  return (_action == Accept ? "accept" : "reject");
}

/** Returns the address (and mask, if specified) for this policy. */
QString
Policy::address()
{
  QString addrString;
  
  if (_mask) {
    if (_address.isNull()) {
      _address = QHostAddress::Any;
    }
    addrString = _address.toString() + "/" + QString::number(_mask);
  } else if (_address == QHostAddress::Any || _address.isNull()) {
    addrString = "*";
  } else {
    addrString = _address.toString();
  } 
  return addrString;
}

/** Returns the port (or port range, if specified) for this policy. */
QString
Policy::ports()
{
  QString ports = (_fromPort ? QString::number(_fromPort) : "*");
  if (_fromPort && _toPort) {
    ports += "-" + QString::number(_toPort);
  }
  return ports;
}

