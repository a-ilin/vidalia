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
 * \file exitpolicy.h
 * \version $Id$
 */

#ifndef _EXITPOLICY_H
#define _EXITPOLICY_H

#include <QString>
#include <QList>

#include "policy.h"


class ExitPolicy
{
public:
  /** Special exit policy types. */
  enum SpecialExitPolicy {
    Middleman /**< Specifies a middleman-only exit policy. */
  };
  
  /** Default constructor. */
  ExitPolicy();
  /** Creates an exit policy of the given special type. */
  ExitPolicy(SpecialExitPolicy exitPolicy);
  /** Creates an exit policy from the given comma-delimited list of policies. */
  ExitPolicy(QString exitPolicy);

  /** Adds a rule to the exit policy. */
  void addPolicy(Policy policy);
  /** Removes a rule from the exit policy. */
  void removePolicy(Policy policy);
  /** Checks if the current exit policy contains the given rule. */
  bool contains(Policy policy);

  /** Returns the list of policies for this exit policy. */
  QList<Policy> policyList() { return  _exitPolicy; }
  
  /** Converts the exit policy to a format Tor understands. */  
  QString toString();
  
private:
  /** A collection of policies forming the exit policy. */
  QList<Policy> _exitPolicy;
};

#endif

