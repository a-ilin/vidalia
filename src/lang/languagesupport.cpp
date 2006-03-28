/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file languagesupport.cpp
 * \version $Id$
 */

#include <QTranslator>
#include <QLocale>

#include "languagesupport.h"

/** Global list of supported languages and codes. */
QMap<QString, QString> _languages;


/** Initializes the list of supported languages. */
void
LanguageSupport::initialize()
{
  _languages.clear();
  _languages.insert("en",    "English");
  _languages.insert("zh-cn", "\347\256\200\344\275\223\345\255\227");
}

/** Returns the default language code for the system locale. */
QString
LanguageSupport::defaultLanguageCode()
{
  QString localeName = QLocale::system().name();
  QString language   = localeName.mid(0, localeName.indexOf("_"));
  if (language == "zh") {
    language += "-" + localeName.mid(localeName.indexOf("_")+1).toLower();
  }
  return language;
}

/** Returns a list of all supported language codes. (e.g., "en"). */
QStringList
LanguageSupport::languageCodes()
{
  return _languages.keys();
}

/** Returns a list of all supported language names (e.g., "English"). */
QStringList
LanguageSupport::languageNames()
{
  return _languages.values();
}

/** Returns a list of all supported language codes and names. */
QMap<QString, QString>
LanguageSupport::languages()
{
  return _languages;
}

/** Sets the application's translator to the specified language. */
void
LanguageSupport::translate(QString langCode)
{
  QTranslator *translator = new QTranslator();
  translator->load(QString(":/lang/") + langCode);
  QApplication::installTranslator(translator);
}

