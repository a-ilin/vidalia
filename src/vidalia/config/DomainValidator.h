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
** \file DomainValidator.h
** \brief Validates that a given domain name is (probably) valid
*/

#ifndef _DOMAINVALIDATOR_H
#define _DOMAINVALIDATOR_H

#include <QRegExpValidator>


class DomainValidator : public QRegExpValidator
{
  Q_OBJECT

public:
  /** Constructor. */
  DomainValidator(QObject *parent);
};

#endif

