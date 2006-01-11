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

#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow()
{
  /* Set Vidalia's application icon */
  setWindowIcon(QIcon(":/images/tor_on32.png"));
  
  createActions();
  createMenus();
  
  /* Put an icon in the system tray to indicate the status of Tor */
  _trayIcon = new TrayIcon(QPixmap(":/images/tor_on32.png"),
                           tr("Vidalia"), _trayMenu, this);
  _trayIcon->show();
}

/* 
  Binds events to actions which are then 
  used in the tray menu. 
*/
void MainWindow::createActions()
{
  _aboutAct = new QAction(tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));
  
  _exitAct = new QAction(tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

/*
  Creates a QMenu object that is to be used as the system tray
  context menu.
*/
void MainWindow::createMenus()
{
  /* Tray menu */ 
  _trayMenu = new QMenu(this);
  _trayMenu->addAction(tr("Start Tor"));
  _trayMenu->addAction(tr("Stop Tor"));
  _toolsMenu = _trayMenu->addMenu(tr("Tools"));
  _trayMenu->addSeparator();
  _trayMenu->addAction(tr("Configure"));
  _trayMenu->addAction(_aboutAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_exitAct);

  /* Tools menu */
  _toolsMenu->addAction(tr("Bandwidth Graph"));
  _toolsMenu->addAction(tr("Message History"));
  _toolsMenu->addSeparator();
  _signalMenu = _toolsMenu->addMenu(tr("Send Signal"));

  /* Signals menu */
  _signalMenu->addAction(QString(tr("Reload Config")));
  _signalMenu->addAction(QString(tr("Dump Stats")));
  _signalMenu->addAction(QString(tr("Debug Mode")));
  _signalMenu->addAction(QString(tr("Shutdown")));
  _signalMenu->addAction(QString(tr("Kill")));
}

/*
 Creates an instance of AboutDialog
*/
void MainWindow::about()
{
  AboutDialog* a = new AboutDialog(this);
  a->show();
  
}
