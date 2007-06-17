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
 * \file serverpage.h
 * \version $Id$
 * \brief Tor server configuration options
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
  /** Called when the user clicks the bandwidth help button */
  void bandwidthHelp();
  /** Called when the user clicks the exit policy help button */
  void exitPolicyHelp();
	/** Called when the user selects a new value from the rate combo box */
	void rateChanged(int rate);
  /** Called when the user edits the max or average bandwidth limits. */
  void customRateChanged();

private:
  /** Index values of rate values in the bandwidth limits dropdown box. */
  enum BwRateIndex {
    CableDsl256 = 0, /**< Cable/DSL 256 Kbps */
    CableDsl512,     /**< Cable/DSL 512 Kbps */
    CableDsl768,     /**< Cable/DSL 768 Kbps */
    T1CableDsl1500,  /**< T1/Cable/DSL 1.5 Mbps */
    GreaterThan1500, /**< > 1.5 Mbps */
    CustomBwLimits   /**< Custom bandwidth limits */
  };
  
  /** Returns the index of the selected item in lstExitPolicies */
  int selectedIndex();
  
  /** Saves the server's bandwidth average and burst limits. */
  void saveBandwidthLimits();
  /** Loads the server's bandwidth average and burst limits. */
  void loadBandwidthLimits();
  /** Saves the server's exit policies. */
  void saveExitPolicies();
  /** Loads the server's exit policies. */
  void loadExitPolicies();

  /** A TorControl object used to talk to Tor */
  TorControl* _torControl;
  /** A ServerSettings object used to get and set information about how a
   * local Tor server is configured. */
  ServerSettings*  _settings;

  /** Qt Designer generated object */
  Ui::ServerPage ui;
};

#endif

