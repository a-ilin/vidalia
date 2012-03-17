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
** \file HelpTextBrowser.cpp
** \brief Displays an HTML-based help document
*/

#include "HelpTextBrowser.h"
#include "VMessageBox.h"
#include "Vidalia.h"

#include "html.h"

#include <QDir>
#include <QFile>
#include <QDesktopServices>


/** Default constructor. */
HelpTextBrowser::HelpTextBrowser(QWidget *parent)
  : QTextBrowser(parent)
{
  setOpenExternalLinks(false);
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


/** Called when the displayed document is changed. If <b>url</b> specifies
 * an external link, then the user will be prompted for whether they want to
 * open the link in their default browser or not. */
void
HelpTextBrowser::setSource(const QUrl &url)
{
  if (url.scheme() != "qrc" && !url.isRelative()) {
    /* External link. Prompt the user for a response. */
    int ret = VMessageBox::question(this,
                tr("Opening External Link"),
                p(tr("Vidalia can open the link you selected in your default "
                     "Web browser. If your browser is not currently "
                     "configured to use Tor then the request will not be "
                     "anonymous.")) +
                p(tr("Do you want Vidalia to open the link in your Web "
                     "browser?")),
                VMessageBox::Yes|VMessageBox::Default,
                VMessageBox::Cancel|VMessageBox::Cancel);

    if (ret == VMessageBox::Cancel)
      return;

    bool ok = QDesktopServices::openUrl(url);
    if (!ok) {
      VMessageBox::information(this,
        tr("Unable to Open Link"),
        tr("Vidalia was unable to open the selected link in your Web browser. "
           "You can still copy the URL and paste it into your browser."),
        VMessageBox::Ok);
    }
  } else {
    /* Internal link. Just load it like normal. */
    QTextBrowser::setSource(url);
  }
}

