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
** \file PortValidator.h
** \brief Validates that a number is a valid port number
*/

#ifndef _PORTVALIDATOR_H
#define _PORTVALIDATOR_H

#include <QIntValidator>


class PortValidator : public QIntValidator
{
  Q_OBJECT

public:
  /** Constructor. */
  PortValidator(QObject *parent);
  /** Validates that the given port is either a valid port or a "*". */
  QValidator::State validate(QString &input, int &pos) const;
};

#endif

