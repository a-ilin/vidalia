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

#define FONT        QFont(tr("Arial"), 10)

/* Page indices in the QListWidget */
#define PAGE_GENERAL      0
#define PAGE_SERVER       1
#define PAGE_APPEARANCE   2
#define PAGE_ADVANCED     3

/* Columns of the Exit Policy list */
#define COL_ACTION    0
#define COL_ADDRESS   1
#define COL_PORT      2

/* Images for toolbar icons */
#define IMAGE_GENERAL       ":/images/22x22/preferences-system.png"
#define IMAGE_SERVER        ":/images/22x22/network-server.png"
#define IMAGE_APPEARANCE    ":/images/22x22/preferences-desktop-locale.png"
#define IMAGE_ADVANCED      ":/images/22x22/emblem-system.png"
#define IMAGE_SAVE          ":/images/22x22/media-floppy.png"
#define IMAGE_CANCEL        ":/images/22x22/emblem-unreadable.png"

/** Constructor */
ConfigDialog::ConfigDialog(TorControl *torControl,
                           HelpBrowser *helpBrowser,
                           QWidget* parent)
: QMainWindow(parent)
{
  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);
  
  _generalPage = new GeneralPage(ui.stackPages);
  _serverPage = new ServerPage(torControl, helpBrowser, ui.stackPages);
  _advancedPage = new AdvancedPage(ui.stackPages);
  _appearancePage = new AppearancePage(ui.stackPages);
  
  /* Add pages to stacked widget */
  ui.stackPages->insertWidget(PAGE_GENERAL, _generalPage);
  ui.stackPages->insertWidget(PAGE_SERVER, _serverPage);
  ui.stackPages->insertWidget(PAGE_APPEARANCE, _appearancePage);
  ui.stackPages->insertWidget(PAGE_ADVANCED, _advancedPage);
  
  /* Create the toolbar */
  QActionGroup *grp = new QActionGroup(this);
  connect(grp, SIGNAL(triggered(QAction *)), this, SLOT(showPage(QAction *)));

  _actionGeneral = new QAction(QIcon(IMAGE_GENERAL),
                                      tr("General"), grp);
  _actionGeneral->setCheckable(true);
  _actionGeneral->setFont(FONT);
  
  _actionServer = new QAction(QIcon(IMAGE_SERVER),
                                     tr("Server"), grp);
  _actionServer->setCheckable(true);
  _actionServer->setFont(FONT);
  
  _actionAppearance = new QAction(QIcon(IMAGE_APPEARANCE),
                                          tr("Appearance"), grp);
  _actionAppearance->setCheckable(true);
  _actionAppearance->setFont(FONT);
  
  _actionAdvanced = new QAction(QIcon(IMAGE_ADVANCED),
                                       tr("Advanced"), grp);
  _actionAdvanced->setCheckable(true);
  _actionAdvanced->setFont(FONT);

  ui.toolBar->addActions(grp->actions());
  ui.toolBar->addSeparator();
  
  _actionSave = new QAction(QIcon(IMAGE_SAVE),
                                   tr("Save"), grp);
  _actionSave->setFont(FONT);
  
  _actionCancel = new QAction(QIcon(IMAGE_CANCEL),
                                     tr("Cancel"), grp);
  _actionCancel->setFont(FONT);

  ui.toolBar->addAction(_actionSave);
  ui.toolBar->addAction(_actionCancel);
  
  /* Bind events to actions */
  connect(_actionCancel, SIGNAL(triggered()), this, SLOT(cancelChanges()));
  connect(_actionSave, SIGNAL(triggered()), this, SLOT(saveChanges()));

  /* Set General Settings selected */
  _actionGeneral->setChecked(true);
  ui.stackPages->setCurrentIndex(PAGE_GENERAL);
}

/** Overloads the default show so we can load settings */
void
ConfigDialog::show()
{
  /* Load saved settings */
  loadSettings();

  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    QMainWindow::activateWindow();
    QMainWindow::raise();
  }
}

/** Loads the saved ConfigDialog settings. */
void
ConfigDialog::loadSettings()
{
  /* Load General settings */
  _generalPage->loadSettings();
  /* Load Server settings */
  _serverPage->loadSettings();
  /* Load Advanced settings */
  _advancedPage->loadSettings();
  /* Load Appearance settings */
  _appearancePage->loadSettings();
}

/** Cancels changes made to settings. */
void
ConfigDialog::cancelChanges()
{
  /* Reload saved settings and exit */
  loadSettings();
  
  QMainWindow::close();
}

/** Saves changes made to settings. */
void
ConfigDialog::saveChanges()
{
  QString errmsg;
  
  /* Save the settings and exit */
  _generalPage->saveChanges();
  _advancedPage->saveChanges();
  _appearancePage->saveChanges();

  /* Try to save the user's server settings. If something goes awry, then
   * notify the user, don't save their settings, and show them the offending
   * page. */
  if (!_serverPage->saveChanges(&errmsg)) {
    QMessageBox::warning(this, 
      tr("Error Saving Server Configuration"),
      tr("Vidalia encountered an error applying your "
         "server configuration.\n\n") + errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  QMainWindow::close();
}

/** Shows General page */
void
ConfigDialog::showPage(QAction *page)
{
  if (page == _actionGeneral) {
    ui.stackPages->setCurrentIndex(PAGE_GENERAL);
  } else if (page == _actionServer) {
    ui.stackPages->setCurrentIndex(PAGE_SERVER);
  } else if (page == _actionAppearance) {
    ui.stackPages->setCurrentIndex(PAGE_APPEARANCE);
  } else {
    ui.stackPages->setCurrentIndex(PAGE_ADVANCED);
  }
}

