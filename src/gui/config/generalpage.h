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
 * \file generalpage.h
 * \version $Id$
 */

#ifndef _GENERALPAGE_H
#define _GENERALPAGE_H

#include <QFileDialog>
#include <config/vidaliasettings.h>
#include <config/torsettings.h>

#include "configpage.h"
#include "ui_generalpage.h"

class GeneralPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  GeneralPage(QWidget *parent = 0);
  /** Default Destructor */
  ~GeneralPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

private slots:
  /** Called when the user clicks "Browse" */
  void browseTorPath();
  
private:
  /* A VidaliaSettings object used for saving/loading vidalia settings */
  VidaliaSettings *_vidaliaSettings;
  /* A TorSettings ovject used for saving/loading tor settings */
  TorSettings *_torSettings;
  /** Qt Designer generated object */
  Ui::GeneralPage ui;
};

#endif

