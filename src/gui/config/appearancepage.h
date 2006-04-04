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
 * \file appearancepage.h
 * \version $Id: appearancepage.h 530 2006-03-30 19:09:35Z hipplej $
 */

#ifndef _APPEARANCEPAGE_H
#define _APPEARANCEPAGE_H

#include <QStyleFactory>
#include <QLineEdit>

#include <config/vidaliasettings.h>
#include <lang/languagesupport.h>

#include "configpage.h"
#include "ui_appearancepage.h"

class AppearancePage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  AppearancePage(QWidget *parent = 0);
  /** Default Destructor */
  ~AppearancePage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();
  
private:
  /** A VidaliaSettings object used for saving/loading settings */
  VidaliaSettings* _settings;

  /** Qt Designer generated object */
  Ui::AppearancePage ui;
};

#endif
