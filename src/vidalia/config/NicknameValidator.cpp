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
** \file NicknameValidator.cpp
** \brief Validates that a server nickname contains only valid characters
*/

#include "NicknameValidator.h"
#include "stringutil.h"

/** Set of characters that are valid in a server's nickname. */
#define VALID_NICKNAME_CHARS \
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"


/** Constructor. */
NicknameValidator::NicknameValidator(QObject *parent)
  : QValidator(parent)
{
}

/** Validates the given input contains only valid nickname characters starting
 * at the specified position. */
QValidator::State
NicknameValidator::validate(QString &input, int &pos) const
{
  Q_UNUSED(pos);

  /* Make sure the input only contains valid characters. If any characters
   * were removed, then we know the input contained invalid characters. */
  QString validString = ensure_valid_chars(input, VALID_NICKNAME_CHARS);
  return (validString.length() == input.length() ? QValidator::Acceptable
                                                 : QValidator::Invalid);
}

