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
** \file helptextbrowser.h
** \version $Id$ 
** \brief Displays an HTML-based help document
*/

#ifndef _HELPTEXTBROWSER_H
#define _HELPTEXTBROWSER_H

#include <QTextBrowser>
#include <QVariant>


class HelpTextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  /** Default constructor. */
  HelpTextBrowser(QWidget *parent = 0);
  /** Loads a resource into the browser. */
  QVariant loadResource(int type, const QUrl &name);
};

#endif

