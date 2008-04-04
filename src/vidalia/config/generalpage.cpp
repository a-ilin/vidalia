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
** \file generalpage.cpp
** \version $Id$
** \brief General Tor and Vidalia configuration options
*/

#include "generalpage.h"


/** Constructor */
GeneralPage::GeneralPage(QWidget *parent)
: ConfigPage(parent, tr("General"))
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Create settings objects */
  _vidaliaSettings = new VidaliaSettings;
  _torSettings = new TorSettings;
  
  /* Bind event to actions */
  connect(ui.btnBrowseTorExecutable, SIGNAL(clicked()), 
          this, SLOT(browseTorPath()));

  /* Hide platform specific features */
#ifndef Q_WS_WIN
  ui.chkRunVidaliaAtSystemStartup->setVisible(false);
#endif
}

/** Destructor */
GeneralPage::~GeneralPage()
{
  delete _vidaliaSettings;
  delete _torSettings;
}

/* Open a QFileDialog to browse for Tor executable */
void
GeneralPage::browseTorPath()
{
#if defined(Q_OS_WIN32)
  QString filter = tr("Executables (*.exe)");
#else
  QString filter = "";
#endif
 
  /* Prompt the user for an executable file. If we're on windows, filter for
   * only .exe files. */
  QString filename = QDir::convertSeparators(
                          QFileDialog::getOpenFileName(this,
                              tr("Select Path to Tor"), 
                              ui.lineTorExecutable->text(),
                              filter));
  if (!filename.isEmpty()) {
    ui.lineTorExecutable->setText(filename);
  }
}

/* Saves all settings for this page */
bool
GeneralPage::save(QString &errmsg)
{
  QString torExecutable = ui.lineTorExecutable->text();
  if (torExecutable.isEmpty()) {
    errmsg = tr("You must specify the name of your Tor executable.");
    return false;
  }
  _torSettings->setExecutable(torExecutable);
  _vidaliaSettings->setRunTorAtStart(ui.chkRunTorAtVidaliaStartup->isChecked());
  _vidaliaSettings->setRunVidaliaOnBoot(
    ui.chkRunVidaliaAtSystemStartup->isChecked());
  return true;
}

/* Loads previously saved settings */
void
GeneralPage::load()
{
  ui.lineTorExecutable->setText(_torSettings->getExecutable());
  ui.chkRunTorAtVidaliaStartup->setChecked(_vidaliaSettings->runTorAtStart());
  ui.chkRunVidaliaAtSystemStartup->setChecked(
    _vidaliaSettings->runVidaliaOnBoot());
}

