/****************************************************************
 *  $Id$
 * 
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

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include "ui_aboutdialog.h"
#include "../../config/vidaliasettings.h"
#include "../../control/torcontrol.h"

class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor **/
  AboutDialog(TorControl *torControl, QWidget *parent = 0);
  
  /** Default destructor **/
  ~AboutDialog();
  
  /** Overriden QWidget.show() **/
  void show();

private:
  /** Qt Designer generated QObject **/
  Ui::AboutDialog ui;

  /** TorControl for access to Tor version info **/
  TorControl *_torControl;
};

#endif
