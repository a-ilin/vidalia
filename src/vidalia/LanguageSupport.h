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
** \file LanguageSupport.h
** \brief Contains languages supported by Vidalia
*/

#ifndef _LANGUAGESUPPORT_H
#define _LANGUAGESUPPORT_H

#include <QStringList>
#include <QMap>


class LanguageSupport
{
public:
  /** Returns the default language code for the system locale. */
  static QString defaultLanguageCode();
  /** Returns the language code for a given language name. */
  static QString languageCode(const QString &languageName);
  /** Returns a list of all supported language codes (e.g., "en"). */
  static QStringList languageCodes();
  /** Returns the language name for a given language code. */
  static QString languageName(const QString &languageCode);
  /** Returns a list of all supported language names (e.g., "English"). */
  static QStringList languageNames();
  /** Returns a list of all supported language codes and names. */
  static QMap<QString, QString> languages();
  /** Returns true if we understand the given language code. */
  static bool isValidLanguageCode(const QString &languageCode);
};

#endif

