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
** \file Local8BitStringValidator.cpp
** \brief Validates that a given string contains only characters capable of
** being represented in the current local 8-bit character encoding.
*/

#include "Local8BitStringValidator.h"


/** Constructor. */
Local8BitStringValidator::Local8BitStringValidator(QObject *parent)
  : QValidator(parent)
{
  _codec = QTextCodec::codecForLocale();
}

/** Validates the given input contains only valid nickname characters starting
 * at the specified position. */
QValidator::State
Local8BitStringValidator::validate(QString &input, int &pos) const
{
  Q_UNUSED(pos);

  if (_codec->canEncode(input))
    return QValidator::Acceptable;
  return QValidator::Invalid;
}

bool
Local8BitStringValidator::canEncode(const QString &input)
{
  return QTextCodec::codecForLocale()->canEncode(input);
}
