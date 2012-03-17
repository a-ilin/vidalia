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
** \file ExitPolicy.h
** \brief Collection of Policy objects representing an exit policy
*/

#ifndef _EXITPOLICY_H
#define _EXITPOLICY_H

#include "Policy.h"

#include <QList>
#include <QString>
#include <QStringList>


class ExitPolicy
{
public:
  /** Special exit policy types. */
  enum SpecialExitPolicy {
    Default,  /**< Specifies the default exit policy. */
    Middleman /**< Specifies a middleman-only exit policy. */
  };

  /** Default constructor. */
  ExitPolicy();
  /** Creates an exit policy of the given special type. */
  ExitPolicy(SpecialExitPolicy exitPolicy);
  /** Creates an exit policy from the given comma-delimited list of policies. */
  ExitPolicy(QString exitPolicy);

  /** Adds the ports specified in <b>portList</b> to a list of ports accepted
   * by this exit policy. Ports may be given either individually or as ranges. */
  void addAcceptedPorts(QStringList portList);
  /** Returns true if this exit policy accepts all ports specified in
   * <b>portList</b>. Ports in <b>portList</b> may be given either individually
   * or as ranges. */
  bool acceptsPorts(QStringList portList);
  /** Adds the ports specified in <b>portList</b> to a list of ports rejected
   * by this exit policy. Ports may be given either individually or as ranges. */
  void addRejectedPorts(QStringList portList);
  /** Returns true if this exit policy rejects all ports specified in
   * <b>portList</b>. Ports in <b>portList</b> may be given either individually
   * or as ranges. */
  bool rejectsPorts(QStringList portList);

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

