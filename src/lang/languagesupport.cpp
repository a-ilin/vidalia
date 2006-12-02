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
 * \brief Contains languages supported by Vidalia
 */

#include <QTranslator>
#include <QLocale>
#include <vidalia.h>

#include "languagesupport.h"


/** Initializes the list of available languages. */
QMap<QString, QString>
LanguageSupport::languages()
{
  static QMap<QString, QString> languages;
  if (languages.isEmpty()) {
    languages.insert("en",    "English");
    languages.insert("bg",
      QString::fromUtf8("\320\221\321\212\320\273\320\263\320"
                        "\260\321\200\321\201\320\272\320\270"));
    languages.insert("de",    "Deutsch");
    languages.insert("es",    
      QString::fromUtf8("espa\303\261ol"));
    languages.insert("fa",
      QString::fromUtf8("\331\201\330\247\330\261\330\263\333\214"));
    languages.insert("fi",    "suomi");
    languages.insert("fr",    
      QString::fromUtf8("fran\303\247ais"));
    languages.insert("hu",    "magyar nyelv");
    languages.insert("it",    "Italiano");
    languages.insert("nb",    
      QString::fromUtf8("Bokm\303\245l"));
    languages.insert("nl",    "Nederlands");
    languages.insert("pl",    "Polski");
    languages.insert("pt",    
      QString::fromUtf8("Portugu\303\252s"));
    languages.insert("ru",
      QString::fromUtf8("\320\240\321\203\321\201\321\201\320\272\320\270\320\271"));
    languages.insert("zh-cn", 
      QString::fromUtf8("\347\256\200\344\275\223\345\255\227"));
  }
  return languages;
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
  if (!isValidLanguageCode(language)) {
    language = "en";
  }
  return language;
}

/** Returns the language code for a given language name. */
QString
LanguageSupport::languageCode(QString languageName)
{
  return languages().key(languageName);
}

/** Returns a list of all supported language codes. (e.g., "en"). */
QStringList
LanguageSupport::languageCodes()
{
  return languages().keys();
}

/** Returns the language name for a given language code. */
QString
LanguageSupport::languageName(QString languageCode)
{
  return languages().value(languageCode);
}

/** Returns a list of all supported language names (e.g., "English"). */
QStringList
LanguageSupport::languageNames()
{
  return languages().values();
}

/** Returns true if we understand the given language code. */
bool
LanguageSupport::isValidLanguageCode(QString code)
{
  return languageCodes().contains(code.toLower());
}

/** Sets the application's translator to the specified language. */
bool
LanguageSupport::translate(QString langCode)
{
  if (isValidLanguageCode(langCode)) {
    QTranslator *translator = new QTranslator();
    langCode = langCode.toLower();
    if (translator->load(QString(":/lang/") + langCode)) {
      QApplication::installTranslator(translator);
      if (langCode == "fa") {
        vApp->setLayoutDirection(Qt::RightToLeft);
      }
      return true;
    }
    delete translator;
  }
  return false;
}

