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
 * \file exitpolicy.cpp
 * \version $Id$
 * \brief Collection of Policy objects representing an exit policy 
 */

#include <QStringList>

#include "exitpolicy.h"


/** Default constructor. */
ExitPolicy::ExitPolicy()
{
}

/** Constructor. Creates an exit policy based on the given type. */
ExitPolicy::ExitPolicy(SpecialExitPolicy exitPolicy)
{
  if (exitPolicy == Middleman) {
    _exitPolicy << Policy(Policy::RejectAll);
  } else if (exitPolicy == Default) {
    _exitPolicy << Policy("reject *:25")
                << Policy("reject *:119")
                << Policy("reject *:135-139")
                << Policy("reject *:445")
                << Policy("reject *:465")
                << Policy("reject *:587")
                << Policy("reject *:1214")
                << Policy("reject *:4661-4666")
                << Policy("reject *:6346-6429")
                << Policy("reject *:6699")
                << Policy("reject *:6881-6999")
                << Policy("accept *:*");
  }
}

/** Parses the given string for a comma-delimited list of policies and 
 * adds them to this this policy. */
ExitPolicy::ExitPolicy(QString exitPolicy)
{
  if (!exitPolicy.isEmpty()) {
    QStringList policyList = exitPolicy.split(",");
    foreach(QString policy, policyList) {
      addPolicy(Policy(policy));
    }
  }
}

/** Adds a policy to this exit policy. */
void
ExitPolicy::addPolicy(Policy policy)
{
  if (!contains(policy)) {
    _exitPolicy << policy;
  }
}

/** Removes a policy from this exit policy. */
void
ExitPolicy::removePolicy(Policy policy)
{
  for (int i = 0; i < _exitPolicy.size(); i++) {
    if (policy == _exitPolicy.at(i)) {
      _exitPolicy.removeAt(i);
      return;
    }
  }
}

/** Adds the ports specified in <b>portList</b> to a list of ports accepted
 * by this exit policy. Ports may be given either individually or as ranges. */
void
ExitPolicy::addAcceptedPorts(QStringList portList)
{
  foreach (QString port, portList) {
    addPolicy(Policy("accept *:" + port));
  }
}

/** Returns true if this exit policy accepts all ports specified in
 * <b>portList</b>. Ports in <b>portList</b> may be given either individually
 * or in ranges (e.g., "6660-6669"). */
bool
ExitPolicy::acceptsPorts(QStringList portList)
{
  foreach (QString port, portList) {
    if (!contains(Policy("accept *:" + port))) {
      return false;
    }
  }
  return true;
}

/** Adds the ports specified in <b>portList</b> to a list of ports rejected
 * by this exit policy. Ports may be given either individually or as ranges. */
void
ExitPolicy::addRejectedPorts(QStringList portList)
{
  foreach (QString port, portList) {
    addPolicy(Policy("reject *:" + port));
  }
}

/** Returns true if this exit policy rejects all ports specified in
 * <b>portList</b>. Ports in <b>portList</b> may be given either individually
 * or in ranges (e.g., "6660-6669"). */
bool
ExitPolicy::rejectsPorts(QStringList portList)
{
  foreach (QString port, portList) {
    if (!contains(Policy("reject *:" + port))) {
      return false;
    }
  }
  return true;
}

/** Returns true if this exit policy contains the given policy. */ 
bool
ExitPolicy::contains(Policy policy)
{
  Policy acceptAll(Policy::AcceptAll);
  Policy rejectAll(Policy::RejectAll);
  
  /* Check for this policy item in the explicitly defined policies */
  foreach (Policy p, _exitPolicy) {
    if (p.matches(policy)) {
      return true;
    }
    if ((p == acceptAll) || (p == rejectAll)) {
      /* This exit policy replaces the default policy, so stop checking */
      return false;
    }
  }
  /* Now check the default exit policy */
  foreach (Policy p, ExitPolicy(Default).policyList()) {
    if (p.matches(policy)) {
      return true;
    }
  }
  return false;
}

/** Converts the exit policy to a format Tor understands. */
QString
ExitPolicy::toString()
{
  QStringList policyList;
  foreach (Policy policy, _exitPolicy) {
    policyList << policy.toString();
  }
  return policyList.join(",");
}

