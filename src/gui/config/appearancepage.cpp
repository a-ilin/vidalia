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
 * \file appearancepage.cpp
 * \version $Id: appearancepage.cpp 530 2006-03-30 19:09:35Z hipplej $
 */

#include "appearancepage.h"

/** Default Constructor */
AppearancePage::AppearancePage(QWidget *parent)
: QWidget(parent)
{
  /* Invoke Designer-generated object setup routine */
  ui.setupUi(this);

  /* Create VidaliaSettings object */
  _settings = new VidaliaSettings();
}

/** Destructor */
AppearancePage::~AppearancePage()
{
  delete _settings;
}

/** Saves the changes on this page */
void
AppearancePage::saveChanges()
{

}
  
/** Loads the settings for this page */
void
AppearancePage::loadSettings()
{

}

