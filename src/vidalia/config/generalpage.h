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
** \file generalpage.h
** \version $Id$
** \brief General Tor and Vidalia configuration options
*/

#ifndef _GENERALPAGE_H
#define _GENERALPAGE_H

#include <QFileDialog>
#include <config/vidaliasettings.h>
#include <config/torsettings.h>

#include "configpage.h"
#include "ui_generalpage.h"

class GeneralPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  GeneralPage(QWidget *parent = 0);
  /** Default Destructor */
  ~GeneralPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

private slots:
  /** Called when the user clicks "Browse" */
  void browseTorPath();
  
private:
  /* A VidaliaSettings object used for saving/loading vidalia settings */
  VidaliaSettings *_vidaliaSettings;
  /* A TorSettings ovject used for saving/loading tor settings */
  TorSettings *_torSettings;
  /** Qt Designer generated object */
  Ui::GeneralPage ui;
};

#endif

