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
** \file AboutDialog.h
** \brief Displays information about Vidalia, Tor, and Qt
*/

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include "ui_AboutDialog.h"
#include "LicenseDialog.h"


class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor */
  AboutDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

private:
  Ui::AboutDialog ui; /**< Qt Designer generated QObject **/
};

#endif

