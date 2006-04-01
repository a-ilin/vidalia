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

#include <QMainWindow>
#include <QFileDialog>

#include <control/torcontrol.h>

#include "generalpage.h"
#include "serverpage.h"
#include "advancedpage.h"
#include "appearancepage.h"

#include "../help/browser/helpbrowser.h"

#include "ui_configdialog.h"

class ConfigDialog : public QMainWindow
{
  Q_OBJECT

public:
  /** Default Constructor */
  ConfigDialog(TorControl *torControl, QWidget *parent = 0);

public slots:
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
  /** Creates a new action for a config page. */
  QAction* createPageAction(QIcon img, QString text, QActionGroup *group);

  /** Actions that populate the toolbar */
  QAction* _actionSave;
  QAction* _actionCancel;

  /** Qt Designer generated object */
  Ui::ConfigDialog ui;
};

#endif

