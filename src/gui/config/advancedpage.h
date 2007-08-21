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
 * \file advancedpage.h
 * \version $Id$
 * \brief Advanced Tor and Vidalia configuration options
 */

#ifndef _ADVANCEDPAGE_H
#define _ADVANCEDPAGE_H

#include <QFileDialog>
#include <config/torsettings.h>

#include "configpage.h"
#include "ui_advancedpage.h"


class AdvancedPage : public ConfigPage 
{
  Q_OBJECT

public:
  /** Default Constructor */
  AdvancedPage(QWidget *parent = 0);
  /** Default Destructor */
  ~AdvancedPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

private slots:
  /** Called when the user selects a different authentication method from the
   * combo box. */
  void authMethodChanged(int index);
  /** Called when the user clicks "Browse" to choose location of Tor config 
   * file */
  void browseTorConfig();
  /** Called when the user clicks "Browse" to choose the location of Tor's
   * data directory. */
  void browseTorDataDirectory();

private:
  /** Returns the authentication method for the given <b>index</b>. */
  TorSettings::AuthenticationMethod indexToAuthMethod(int index);
  /** Returns the index in the authentication methods combo box for the given
   * authentication <b>method</b>. */
  int authMethodToIndex(TorSettings::AuthenticationMethod method);
  
#if defined(Q_WS_WIN)
  /** Installs or removes the Tor service as necessary */
  void setupService(bool useService);
#endif

  /** A TorSettings object used to save/load settings */
  TorSettings* _settings;
  /** Qt Designer generated object */
  Ui::AdvancedPage ui;
};

#endif

