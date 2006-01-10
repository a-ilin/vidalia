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

#include "traymenu.h"

/*
  Creates a QMenu object that is to be used as the system tray
  context menu.
*/
TrayMenu::TrayMenu(QWidget* parent) 
: QMenu(parent)
{
  _toolsMenu = new QMenu(QString(tr("Tools")), this);
  _signalMenu = new QMenu(QString(tr("Send Signal")), _toolsMenu);

  /* Signals menu */
  _signalMenu->addAction(QString(tr("Reload Config")));
  _signalMenu->addAction(QString(tr("Dump Stats")));
  _signalMenu->addAction(QString(tr("Debug Mode")));
  _signalMenu->addAction(QString(tr("Shutdown")));
  _signalMenu->addAction(QString(tr("Kill")));
  
  /* Tools menu */
  _toolsMenu->addAction(QString(tr("Bandwidth Graph")));
  _toolsMenu->addAction(QString(tr("Message History")));
  _toolsMenu->addSeparator();
  _toolsMenu->addMenu(_signalMenu);

  /* Systray context menu */
  addAction(QString(tr("Start Tor")));
  addAction(QString(tr("Stop Tor")));
  addMenu(_toolsMenu);
  addSeparator();
  addAction(QString(tr("Configure")));
  addAction(QString(tr("About")));
  addSeparator();
  addAction(QString(tr("Exit")));
}

TrayMenu::~TrayMenu()
{
}
