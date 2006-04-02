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
    addPolicy(Policy(Policy::RejectAll));
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

/** Returns true if this exit policy contains the given policy. */ 
bool
ExitPolicy::contains(Policy policy)
{
  foreach (Policy p, _exitPolicy) {
    if (p == policy) {
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

