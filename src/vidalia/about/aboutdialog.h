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
** \file aboutdialog.h
** \version $Id$
** \brief Displays information about Vidalia, Tor, and Qt
*/

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include <torcontrol.h>
#include <vidaliawindow.h>
#include "ui_aboutdialog.h"


class AboutDialog : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Default constructor **/
  AboutDialog(QWidget *parent = 0, Qt::WFlags flags = 0);

public slots:
  /** Overriden VidaliaWindow::showWindow() */
  void showWindow();

private:
  /** Loads the license file */
  void loadLicense();

  /** Qt Designer generated QObject **/
  Ui::AboutDialog ui;
  /** TorControl for access to Tor version info **/
  TorControl *_torControl;
};

#endif

