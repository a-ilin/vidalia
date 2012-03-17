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
** \file PortValidator.cpp
** \brief Validates that a number is a valid port number
*/

#include "PortValidator.h"

#define MIN_PORT  1     /**< Minimum valid port. */
#define MAX_PORT  65535 /**< Maximum valid port. */
#define MATCH_ALL "*"   /**< Matches all ports. */


/** Constructor. */
PortValidator::PortValidator(QObject *parent)
 : QIntValidator(MIN_PORT, MAX_PORT, parent)
{
}

/** Validates that the given port is either a valid port or a "*". */
QValidator::State
PortValidator::validate(QString &input, int &pos) const
{
  if (input == MATCH_ALL) {
    return QValidator::Acceptable;
  }
  return QIntValidator::validate(input, pos);
}

