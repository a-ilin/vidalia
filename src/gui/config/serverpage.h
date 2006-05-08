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

#ifndef _SERVERPAGE_H
#define _SERVERPAGE_H

#include <QMessageBox>
#include <QTimer>
#include <control/torcontrol.h>
#include <config/serversettings.h>
#include <config/exitpolicy.h>
#include <gui/help/browser/helpbrowser.h>

#include "configpage.h"
#include "ui_serverpage.h"

class ServerPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  ServerPage(QWidget *parent = 0);
  /** Default Destructor */
  ~ServerPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

private slots:
  /** Called when the user clicks "Get Address" to guess our local IP */
  void getServerAddress();
  /** Called when the user clicks the "Add Policy" button */
  void addPolicy();
  /** Called when the user clicks the "Remove Policy" button */
  void removePolicy();
  /** Called when the user clicks the "Raise Priority" button */
  void raisePriority();
  /** Called when the user clicks the "Lower Priority" button */
  void lowerPriority();
  /** Called when the user clicks the exit policy help button */
  void exitHelp();
  /** Called when the user's public IP address needs to be updated. */
  void updateServerIP();
  
private:
  /** Attempts to find the server's public IP address */
  void getServerPublicIP(); 
  /** Returns the index of the selected item in lstExitPolicies */
  int selectedIndex();
  /** Adds a new exit policy to the exit policy list */
  void addPolicyItem(Policy policy);
  /** Saves the policy specified in item to the exitPolicy */
  void savePolicy(QTreeWidgetItem *item, ExitPolicy &exitPolicy);
  /** Enables or disables the automatic update timer. */
  void setAutoUpdateTimer(bool enabled);

  /** A TorControl object used to talk to Tor */
  TorControl* _torControl;
  /** A ServerSettings object used to get and set information about how a
   * local Tor server is configured. */
  ServerSettings*  _settings;
  /** A timer that tells us when it's time to check if our server IP changed.*/
  QTimer* _autoUpdateTimer;

  /** Qt Designer generated object */
  Ui::ServerPage ui;
};

#endif

