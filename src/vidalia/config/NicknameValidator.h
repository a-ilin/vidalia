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
** \file NicknameValidator.h
** \brief Validates that a server nickname contains only valid characters
*/

#ifndef _NICKNAMEVALIDATOR_H
#define _NICKNAMEVALIDATOR_H

#include <QValidator>


class NicknameValidator : public QValidator
{
  Q_OBJECT

public:
  /** Constructor. */
  NicknameValidator(QObject *parent);
  /** Validates the given input at the specified position. */
  QValidator::State validate(QString &input, int &pos) const;
};

#endif

