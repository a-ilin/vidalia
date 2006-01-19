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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>

#include "../control/torcontrol.h"
#include "tray/trayicon.h"
#include "aboutdialog/aboutdialog.h"
#include "messagelog/messagelog.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor */
  MainWindow();
  /** Default destructor */
  ~MainWindow();


public slots:
  /** Overrides the default show() slot to display the default menubar */
  void show();


private slots:
  void start();
  void started();
  void stop();
  void stopped(int errorCode, QProcess::ExitStatus exitStatus);
  void about();
  void message();
  void close();


private:
  /** Create the tray popup menu and it's submenus */
  void createMenus();
  /** Create the actions on the tray menu or menubar */
  void createActions();
  
  /** Creates a default menubar on Mac */
  void createMenuBar();
  /** Removes the default menubar on Mac */
  void removeMenuBar();
  
  /* Used to determine if the Tor process exiting was intentional or not */
  bool _isIntentionalExit;
 
  /** Instantiates a window for logging messages from Tor */
  MessageLog* _messageLog;
  
  /** A TorControl object that handles communication with Tor */
  TorControl* _torControl;
  
  /** Instance of a tray icon that will appear in the system tray */
  TrayIcon* _trayIcon;
  
  /** Define the popup menus for the system tray icon */
  QMenu* _trayMenu;
  QMenu* _toolsMenu;
  
  /** Defines the actions for the tray menu */
  QAction* _startAct;
  QAction* _stopAct;
  QAction* _configAct;
  QAction* _aboutAct;
  QAction* _exitAct;
  QAction* _bandwidthAct;
  QAction* _messageAct;

/* GUI elements unique on Mac so we can use the system menubar */
#if defined(Q_WS_MAC)
  QMenuBar* _menuBar;
  QMenu* _fileMenu;
  QMenu* _torMenu;
  QMenu* _helpMenu;
#endif
};

#endif

