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
 * \brief Contains a series of Vidalia and Tor configuration pages
 */

#ifndef _CONFIGDIALOG_H
#define _CONFIGDIALOG_H

#include <QMainWindow>
#include <QFileDialog>
#include <vidaliawindow.h>

#include "generalpage.h"
#include "networkpage.h"
#include "serverpage.h"
#include "advancedpage.h"
#include "appearancepage.h"

#include "ui_configdialog.h"


class ConfigDialog : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Config dialog pages. */
  enum Page {
    General = 0,  /** General configuration page. */
    Network,      /** Network configuration page. */
    Server,       /** Server configuration page. */
    Appearance,   /** Appearance configuration page. */
    Advanced      /** Advanced configuration page. */
  };

  /** Default Constructor */
  ConfigDialog(QWidget *parent = 0);

public slots:
  /** Shows the config dialog with focus set to the given page. */
  void showWindow(Page page = General);

private slots:
  /** Called when user clicks "Save Settings". Saves their settings to
   * Vidalia's configuration file. */
  void saveChanges();
  /** Called after Vidalia has authenticated to Tor and applies any changes
   * made since the last time they were applied. */
  void applyChanges();
  /** Sends Tor a SAVECONF to write its configuration to disk. If the
   * SAVECONF is successful, then all settings are considered to be
   * applied. */
  void saveConf();
  /** Called when a ConfigPage in the dialog requests help on a specific
   * <b>topic</b>. */
  void help(const QString &topic);
  /** Shows general help information for whichever settings page the user is
   * currently viewing. */
  void help();

private:
  /** Loads the current configuration settings */
  void loadSettings();
  /** Creates a new action for a config page. */
  QAction* createPageAction(QIcon img, QString text, QActionGroup *group);
  /** Adds a new action to the toolbar. */
  void addAction(QAction *action, const char *slot = 0);
  
  /** Qt Designer generated object */
  Ui::ConfigDialog ui;
};

#endif

