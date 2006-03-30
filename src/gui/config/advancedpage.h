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
 */

#ifndef _ADVANCEDPAGE_H
#define _ADVANCEDPAGE_H

#include <QFileDialog>
#include <config/torsettings.h>

#include "ui_advancedpage.h"

class AdvancedPage : public QWidget
{
  Q_OBJECT

public:
  /** Default Constructor */
  AdvancedPage(QWidget *parent = 0);
  /** Saves the changes on this page */
  void saveChanges(TorSettings *torsettings);
  /** Loads the settings for this page */
  void loadSettings(TorSettings *torSettings);

private slots:
  /** Called when user clicks "Browse" to choose location of Tor config file */
  void browseTorConfig();
  
private:
  /** Qt Designer generated object */
  Ui::AdvancedPage ui;
};

#endif

