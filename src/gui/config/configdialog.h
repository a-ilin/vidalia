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
 * \file configdialog.h
 * \version $Id$
 */

#ifndef _CONFIGDIALOG_H
#define _CONFIGDIALOG_H

#include <QFileDialog>

#include <control/torcontrol.h>
#include <config/vidaliasettings.h>
#include <config/torsettings.h>

#include "generalpage.h"
#include "serverpage.h"
#include "advancedpage.h"

#include "../help/browser/helpbrowser.h"

#include "ui_configdialog.h"

class ConfigDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default Constructor */
  ConfigDialog(TorControl *torControl,
               HelpBrowser *helpBrowser,
               QWidget *parent = 0);
  /** Default Destructor */
  ~ConfigDialog();

public slots:
  /** Called when user selects an item from the settings page list */
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  /** Called when this dialog is to be displayed */
  void show();
  
private slots:
  /** Called when user clicks "Cancel" */
  void cancelChanges();
  /** Called when user clicks "Save Settings" */
  void saveChanges();

private:
  /** Loads the current configuration settings */
  void loadSettings();

  /** Pages that populate the stacked widget */
  GeneralPage* _generalPage;
  ServerPage* _serverPage;
  AdvancedPage* _advancedPage;
  
  /** A HelpBrowser object used to show context sensitive help */
  HelpBrowser* _helpBrowser;
  /** A VidaliaSettings object that handles getting and setting Vidalia settings */
  VidaliaSettings* _vidaliaSettings;
  /** A TorSettings object that handles getting and setting Tor-specific settings. */
  TorSettings* _torSettings;

  /** Qt Designer generated object */
  Ui::ConfigDialog ui;
};

#endif

