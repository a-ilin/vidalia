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
 * \file configdialog.cpp
 * \version $Id$
 */

#include <QMessageBox>

#include "configdialog.h"

/* Page indices in the QListWidget */
#define PAGE_GENERAL      0
#define PAGE_SERVER       1
#define PAGE_ADVANCED     2

/* Columns of the Exit Policy list */
#define COL_ACTION    0
#define COL_ADDRESS   1
#define COL_PORT      2

/** Constructor */
ConfigDialog::ConfigDialog(TorControl *torControl,
                           HelpBrowser *helpBrowser,
                           QWidget* parent)
: QDialog(parent)
{
  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Keep a pointer to the HelpBrowser object used to show help */
  _helpBrowser = helpBrowser;
  
  /* Create necessary ConfigDialog QObjects */
  _torSettings = new TorSettings();

  _generalPage = new GeneralPage(ui.stackPages);
  _serverPage = new ServerPage(torControl, helpBrowser, ui.stackPages);
  _advancedPage = new AdvancedPage(ui.stackPages);

  /* Add pages to stacked widget */
  ui.stackPages->insertWidget(PAGE_GENERAL, _generalPage);
  ui.stackPages->insertWidget(PAGE_SERVER, _serverPage);
  ui.stackPages->insertWidget(PAGE_ADVANCED, _advancedPage);
    
  /* Bind events to actions */
  connect(ui.lstPages, 
          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this, SLOT(changePage(QListWidgetItem *, QListWidgetItem *)));
  connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(cancelChanges()));
  connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(saveChanges()));
  
  /* Set General Settings selected */
  ui.stackPages->setCurrentIndex(PAGE_GENERAL);
  ui.lstPages->setItemSelected(ui.lstPages->item(PAGE_GENERAL), true);
}

/** Destructor */
ConfigDialog::~ConfigDialog()
{
  delete _torSettings;
}

/** Overloads the default show so we can load settings */
void
ConfigDialog::show()
{
  /* Load saved settings */
  loadSettings();

  if (!this->isVisible()) {
    QDialog::show();
  } else {
    QDialog::activateWindow();
    QDialog::raise();
  }
}

/** Changes settings page. */
void
ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current) {
    current = previous;
  }
  
  ui.stackPages->setCurrentIndex(ui.lstPages->row(current));
}

/** Loads the saved ConfigDialog settings. */
void
ConfigDialog::loadSettings()
{
  /* Load General settings */
  _generalPage->loadSettings(_torSettings);
  /* Load Server settings */
  _serverPage->loadSettings();
  /* Load Advanced settings */
  _advancedPage->loadSettings(_torSettings);
}

/** Cancels changes made to settings. */
void
ConfigDialog::cancelChanges()
{
  /* Reload saved settings and exit */
  loadSettings();
  
  QDialog::close();
}

/** Saves changes made to settings. */
void
ConfigDialog::saveChanges()
{
  QString errmsg;
  
  /* Save the settings and exit */
  _generalPage->saveChanges(_torSettings);
  _advancedPage->saveChanges(_torSettings);
 
  /* Try to save the user's server settings. If something goes awry, then
   * notify the user, don't save their settings, and show them the offending
   * page. */
  if (!_serverPage->saveChanges(&errmsg)) {
    QMessageBox::warning(this, 
      tr("Error Saving Server Configuration"),
      tr("Vidalia encountered an error applying your "
         "server configuration.\n\n") + errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
    ui.lstPages->setCurrentRow(PAGE_SERVER);
    return;
  }
  QDialog::close();
}

