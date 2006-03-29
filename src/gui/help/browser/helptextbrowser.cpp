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
 * \file helptextbrowser.cpp
 * \version $Id$ 
 */

#include <QFile>

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
    QFile file(":/help/" + name.path());
    if (!file.open(QIODevice::ReadOnly)) {
      return tr("Error opening help file: ") + name.path();
    }
    return QString::fromUtf8(file.readAll());
  }
  /* Everything else, just let QTextBrowser take care of it. */
  return QTextBrowser::loadResource(type, name);
}

