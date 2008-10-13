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
** \file appearancepage.h
** \version $Id$
** \brief Displays Vidalia language and style settings
*/

#ifndef _APPEARANCEPAGE_H
#define _APPEARANCEPAGE_H

#include <QStyleFactory>
#include <QLineEdit>

#include <vidaliasettings.h>
#include <languagesupport.h>

#include "configpage.h"
#include "ui_appearancepage.h"

class AppearancePage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  AppearancePage(QWidget *parent = 0);
  /** Default Destructor */
  ~AppearancePage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private:
  /** A VidaliaSettings object used for saving/loading settings */
  VidaliaSettings* _settings;

  /** Qt Designer generated object */
  Ui::AppearancePage ui;
};

#endif
