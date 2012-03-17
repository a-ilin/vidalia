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
** \file LicenseDialog.cpp
** \brief Displays HTML-formatted license information for Vidalia and related
** software.
*/

#include "LicenseDialog.h"

#include <QFile>
#include <QString>


LicenseDialog::LicenseDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  ui.txtLicense->setHtml(loadHtml(":/docs/license.html"));
  ui.txtCredits->setHtml(loadHtml(":/docs/credits.html"));
}

QString
LicenseDialog::loadHtml(const QString &source) const
{
  QFile file(source);
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    return QString();
  return QString(file.readAll());
}

