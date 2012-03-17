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
** \file DomainValidator.cpp
** \brief Validates that a given domain name is (probably) valid
*/

#include "DomainValidator.h"


/** Regular expression to validate that input is a valid IP address. */
#define DOMAIN_REGEXP \
"^([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}$"


/** Constructor. */
DomainValidator::DomainValidator(QObject *parent)
: QRegExpValidator(QRegExp(DOMAIN_REGEXP), parent)
{
}

