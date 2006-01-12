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
 
  /* Create a new TorControl object, used to communicate with and manipulate Tor */
  _torControl = new TorControl();
  
  /* Put an icon in the system tray to indicate the status of Tor */
  _trayIcon = new TrayIcon(QPixmap(":/images/tor_on32.png"),
                           tr("Tor is Started"), _trayMenu, this);
  _trayIcon->show();
}

MainWindow::~MainWindow()
{
  if (_torControl) {
    delete _torControl;
  }
}

/* 
  Create and bind actions to events. Setup for initial
  tray menu configuration.
*/
void MainWindow::createActions()
{
  _startAct = new QAction(tr("Start"), this);
  connect(_startAct, SIGNAL(triggered()), this, SLOT(start()));
  _startAct->setEnabled(false);
  
  _stopAct = new QAction(tr("Stop"), this);
  connect(_stopAct, SIGNAL(triggered()), this, SLOT(stop()));

  _configAct = new QAction(tr("Configure"), this);
  
  _aboutAct = new QAction(tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));
  
  _exitAct = new QAction(tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _bandwidthAct = new QAction(tr("Bandwidth Usage Graph"), this); 

  _messageAct = new QAction(tr("Log Message History"), this);
}

/*
  Creates a QMenu object that contains QActions
  which compose the system tray menu.
*/
void MainWindow::createMenus()
{
  /* Tray menu */ 
  _trayMenu = new QMenu(this);
  _trayMenu->addAction(_startAct);
  _trayMenu->addAction(_stopAct);
  _toolsMenu = _trayMenu->addMenu(tr("Tools"));
  _trayMenu->addSeparator();
  _trayMenu->addAction(_configAct);
  _trayMenu->addAction(_aboutAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_exitAct);

  /* Tools menu */
  _toolsMenu->addAction(_bandwidthAct);
  _toolsMenu->addAction(_messageAct);
}

/*
 Starts Tor, modifies tray icon and tray menu appropriately
*/
void MainWindow::start()
{
  /* Set correct tray icon */
  _trayIcon->setIcon(QPixmap(":/images/tor_on32.png"));
  
  /* Set ToolTip */
  _trayIcon->setToolTip(QString("Tor is started"));

  /* Set menu actions appropriately */
  _startAct->setEnabled(false);
  _stopAct->setEnabled(true);

  /* FIXME Add code for starting Tor */

}

/*
 Stops Tor, modifies tray icon and tray menu appropriately
*/
void MainWindow::stop()
{
  /* Set correct tray icon */
  _trayIcon->setIcon(QPixmap(":/images/tor_off32.png"));

  /* Set ToolTip */
  _trayIcon->setToolTip(QString("Tor is stopped"));

  /* Set menu actions appropriately */
  _stopAct->setEnabled(false);
  _startAct->setEnabled(true);

  /* FIXME Add code for stopping Tor */
  
}

/*
 Creates an instance of AboutDialog
*/
void MainWindow::about()
{
  AboutDialog* aboutDialog = new AboutDialog(_torControl, this);
  aboutDialog->show();
}

