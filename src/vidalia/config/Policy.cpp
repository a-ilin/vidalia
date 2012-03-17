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
** \file Policy.cpp
** \brief Exit policy parsing
*/

#include "Policy.h"

#include <QStringList>


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
  _mask     = mask;
  _fromPort = fromPort;
  _toPort   = (toPort >= fromPort ? toPort : fromPort);
}

/** Returns true if this policy is identical to <b>policy</b>. */
bool
Policy::operator==(const Policy &policy) const
{
  return ((this->_action   == policy._action)    &&
          (this->_address  == policy._address)   &&
          (this->_mask     == policy._mask)      &&
          (this->_fromPort == policy._fromPort)  &&
          (this->_toPort   == policy._toPort));
}

/** Returns true if this policy matches <b>policy</b>. For example, if this
 * policy is "reject *:6660-6669" and <b>policy</b> is "reject *:6662-6664",
 * this will return true. For strict comparison, use the == operator. */
bool
Policy::matches(const Policy &policy) const
{
  /* This doesn't take into account addr/mask matches yet */
  return ((this->_action   == policy._action)    &&
          (this->_address  == policy._address)   &&
          (this->_mask     == policy._mask)      &&
          (this->_fromPort <= policy._fromPort)  &&
          (this->_toPort   >= policy._toPort));
}

/** Parses the given exit policy string. */
void
Policy::fromString(QString policy)
{
  /* Separate the action and the address/mask/port info */
  QStringList ruleParts = policy.split(" ");
  _action = toAction(ruleParts.at(0));

  /* If some address/mask/port stuff was specified, parse it. */
  if (ruleParts.size() > 1) {
    QStringList addrParts = ruleParts.at(1).split(":");

    /* Parse the address and mask (if specified) */
    QString addr = addrParts.at(0);
    _address.setAddress(addr.mid(0, addr.indexOf("/")));
    if (_address.isNull()) {
      _address = QHostAddress::Any;
    }
    if (addr.contains("/")) {
      _mask = addr.mid(addr.indexOf("/")+1).toUInt();
    }

    /* Parse the specified port range (if specified) */
    if (addrParts.size() > 1) {
      QString ports = addrParts.at(1);
      _fromPort = ports.mid(0, ports.indexOf("-")).toUInt();
      if (ports.contains("-")) {
        _toPort = ports.mid(ports.indexOf("-")+1).toUInt();
      } else {
        _toPort = _fromPort;
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
Policy::toString() const
{
  QString act = (_action == Accept ? "accept" : "reject");
  return act + " " + address() + ":" + ports();
}

/** Converts the given action to a string. This function tolerates both the
 * translated and untranslated forms of the string "accept" and "reject". */
Policy::Action
Policy::toAction(QString action)
{
  action = action.toLower();
  if (action == tr("accept") || action == "accept") {
    return Accept;
  }
  return Reject;
}

/** Returns the action associated with this policy. NOTE: This string will be
 * translated to whatever the current language setting is. */
QString
Policy::action() const
{
  return (_action == Accept ? tr("accept") : tr("reject"));
}

/** Returns the address (and mask, if specified) for this policy. */
QString
Policy::address() const
{
  QString addrString;

  if (_mask) {
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
Policy::ports() const
{
  QString ports = (_fromPort ? QString::number(_fromPort) : "*");
  if (_fromPort && (_toPort > _fromPort)) {
    ports += "-" + QString::number(_toPort);
  }
  return ports;
}

