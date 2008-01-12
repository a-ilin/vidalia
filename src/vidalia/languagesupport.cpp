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
    languages.insert("ar",
      QString::fromUtf8("\330\247\331\204\330\271\330\261\330"
                        "\250\331\212\330\251"));
    languages.insert("bg",
      QString::fromUtf8("\320\221\321\212\320\273\320\263\320"
                        "\260\321\200\321\201\320\272\320\270"));
    languages.insert("cs",    
      QString::fromUtf8("\304\215esky"));
    languages.insert("de",    "Deutsch");
    languages.insert("es",    
      QString::fromUtf8("espa\303\261ol"));
    languages.insert("fa",
      QString::fromUtf8("\331\201\330\247\330\261\330\263\333\214"));
    languages.insert("fi",    "suomi");
    languages.insert("fr",    
      QString::fromUtf8("fran\303\247ais"));
    languages.insert("he",
      QString::fromUtf8("\327\242\327\221\327\250\327\231\327\252"));
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
    languages.insert("sq",    "Shqip");
    languages.insert("sv",    "svenska");
    languages.insert("zh-cn", 
      QString::fromUtf8("\347\256\200\344\275\223\345\255\227"));
    languages.insert("zh-tw", 
      QString::fromUtf8("\347\260\241\351\253\224\345\255\227"));
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
LanguageSupport::languageCode(const QString &languageName)
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
LanguageSupport::languageName(const QString &languageCode)
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
LanguageSupport::isValidLanguageCode(const QString &languageCode)
{
  return languageCodes().contains(languageCode);
}

/** Returns true if <b>languageCode</b> requires a right-to-left layout. */
bool
LanguageSupport::isRightToLeft(const QString &languageCode)
{
  return (!languageCode.compare("ar", Qt::CaseInsensitive) 
            || !languageCode.compare("fa", Qt::CaseInsensitive)
            || !languageCode.compare("he", Qt::CaseInsensitive));
}

/** Sets the application's translator to the specified language. */
bool
LanguageSupport::translate(const QString &languageCode)
{
  QString code = languageCode.toLower();
  if (isValidLanguageCode(code)) {
    QTranslator *translator = new QTranslator(vApp);
    if (translator->load(QString(":/lang/") + code)) {
      QApplication::installTranslator(translator);
      if (isRightToLeft(code))
        vApp->setLayoutDirection(Qt::RightToLeft);
      return true;
    }
    delete translator;
  }
  return false;
}

