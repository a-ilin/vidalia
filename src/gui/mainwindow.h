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
#include "about.h"
#include "messagelog.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();
  
private slots:
  void start();
  void started();
  void stop();
  void stopped(int errorCode, QProcess::ExitStatus exitStatus);
  void about();
  void message();
  void close();

private:
  void createMenus();
  void createActions();

  /* Used to determine if the Tor process exiting was intentional or not */
  bool _isIntentionalExit;
  
  MessageLog* _messageLog;
  
  TorControl* _torControl;
  
  TrayIcon* _trayIcon;
  
  QMenu* _trayMenu;
  QMenu* _toolsMenu;

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

