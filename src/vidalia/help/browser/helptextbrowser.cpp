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
** \file helptextbrowser.cpp
** \version $Id$
** \brief Displays an HTML-based help document
*/

#include <QDir>
#include <QFile>
#include <vidalia.h>

#include "helptextbrowser.h"


/** Default constructor. */
HelpTextBrowser::HelpTextBrowser(QWidget *parent)
: QTextBrowser(parent)
{
}

/** Loads a resource into the browser. If it is an HTML resource, we'll load
 * it as UTF-8, so the special characters in our translations appear properly. */
QVariant
HelpTextBrowser::loadResource(int type, const QUrl &name)
{
  /* If it's an HTML file, we'll handle it ourselves */
  if (type == QTextDocument::HtmlResource) {
    QString helpPath = ":/help/";
    
    /* Fall back to English if there is no translation of the specified help
     * page in the current language. */
    if (!name.path().contains("/")) {
      QString language = Vidalia::language();
      if (!QDir(":/help/" + language).exists())
        language = "en";
      helpPath += language + "/";
    }
    
    QFile file(helpPath + name.path());
    if (!file.open(QIODevice::ReadOnly)) {
      return tr("Error opening help file: ") + name.path();
    }
    return QString::fromUtf8(file.readAll());
  }
  /* Everything else, just let QTextBrowser take care of it. */
  return QTextBrowser::loadResource(type, name);
}

