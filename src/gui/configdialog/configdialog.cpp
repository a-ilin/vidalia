/****************************************************************
 *  $Id$ 
 * 
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

#include "configdialog.h"

/** Constructor */
ConfigDialog::ConfigDialog(QWidget* parent)
: QDialog(parent)
{
  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Create necessary ConfigDialog QObjects */
  _settings = new VidaliaSettings();

  /* Bind events to actions */
  createActions();

  /* Load saved settings */
  loadSettings();
}

/** Destructor */
ConfigDialog::~ConfigDialog()
{
  if (_settings) {
    delete _settings;
  }
}

/** Connects actions to events. */
void
ConfigDialog::createActions()
{
  connect(ui.lstPages, 
          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this, SLOT(changePage(QListWidgetItem *, QListWidgetItem *)));

  connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(cancelChanges()));

  connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(saveChanges()));

  connect(ui.btnBrowseTorPath, SIGNAL(clicked()), this, SLOT(browseTorPath()));

  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));
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
  /* Load the saved settings from VidaliaSettings */
  ui.lineTorPath->setText(_settings->getTorPath());
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
  /* Save the settings and exit */
  _settings->setTorPath(ui.lineTorPath->text());

  QDialog::close();
}

/** Open a QFileDialog to browse for Tor executable. */
void 
ConfigDialog::browseTorPath()
{
  QString filename = QDir::convertSeparators(
                          QFileDialog::getExistingDirectory(this,
                              tr("Select Path to Tor"), 
                              ui.lineTorPath->text()));
  if (!filename.isEmpty()) {
    ui.lineTorPath->setText(filename);
  }
}

/** Open a QFileDialog to browse for Tor config file. */
void
ConfigDialog::browseTorConfig()
{
  QString filename = QDir::convertSeparators(
                          QFileDialog::getSaveFileName(this,
                              tr("Select Tor Configuration File"), "torrc"));
  if (!filename.isEmpty()) {
    ui.lineTorConfig->setText(filename);
  }
}
 
