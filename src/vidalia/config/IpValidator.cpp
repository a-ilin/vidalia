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
** \file IpValidator.cpp
** \brief Validates an entered IP address
*/

#include "IpValidator.h"

/** Regular expression to validate that input is a valid IP address. */
#define IP_REGEXP "\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"\
                  "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"\
                  "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"\
                  "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b"

#define MATCH_ALL "*" /**< Match all IP addresses. */


/** Constructor. */
IpValidator::IpValidator(QObject *parent)
: QRegExpValidator(QRegExp(IP_REGEXP), parent)
{
}

/** Validates the given input is either a valid IP or a "*". */
QValidator::State
IpValidator::validate(QString &input, int &pos) const
{
  if (input == MATCH_ALL) {
    return QValidator::Acceptable;
  }
  return QRegExpValidator::validate(input, pos);
}

/** Validates the given input from position 0. */
QValidator::State
IpValidator::validate(QString &input) const
{
  int discard = 0;
  return validate(input, discard);
}

