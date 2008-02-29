/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file ipvalidator.h
** \version $Id$
** \brief Validates an entered IP address
*/

#ifndef _IPVALIDATOR_H
#define _IPVALIDATOR_H

#include <QRegExpValidator>


class IPValidator : public QRegExpValidator
{
public:
  /** Constructor. */
  IPValidator(QObject *parent);
  /** Validates the given input from position 0. */
  QValidator::State validate(QString &input) const;
  /** Validates the given input is either a valid IP or a "*". */
  QValidator::State validate(QString &input, int &pos) const;
};

#endif

