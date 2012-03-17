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
** \file Local8BitStringValidator.h
** \brief Validates that a given string contains only characters capable of
** being represented in the current local 8-bit character encoding.
*/

#ifndef _LOCAL8BITSTRINGVALIDATOR_H
#define _LOCAL8BITSTRINGVALIDATOR_H

#include <QValidator>
#include <QTextCodec>


class Local8BitStringValidator : public QValidator
{
  Q_OBJECT

public:
  /** Constructor. */
  Local8BitStringValidator(QObject *parent);
  /** Validates the given input at the specified position. */
  QValidator::State validate(QString &input, int &pos) const;

  /** Returns true if <b>input</b> can be encoded with the current local
   * 8-bit character encoding. */
  static bool canEncode(const QString &input);

private:
  QTextCodec* _codec;
};

#endif

