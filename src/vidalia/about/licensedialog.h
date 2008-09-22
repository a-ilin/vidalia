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
** \file licensedialog.h
** \version $Id$
** \brief Displays HTML-formatted license information for Vidalia and related
** software.
*/

#ifndef _LICENSEDIALOG_H
#define _LICENSEDIALOG_H

#include "ui_licensedialog.h"


class LicenseDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  LicenseDialog(QWidget *parent = 0);

protected:
  /** Reads and returns all HTML-formatted text from <b>source</b>.
   */
  virtual QString loadHtml(const QString &source) const;

private:
  Ui::LicenseDialog ui;
};

#endif

