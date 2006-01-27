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
#include "../util/compat.h"
#include "../config/messagetypes.h"

/* On Mac, we need to put the 128x128 icon in the dock */
#if defined(Q_WS_MAC)
#define IMG_TOR_STOPPED    ":/images/tor_off48.png"
#define IMG_TOR_RUNNING    ":/images/tor_on48.png"
#else
#define IMG_TOR_STOPPED    ":/images/tor_off32.png"
#define IMG_TOR_RUNNING    ":/images/tor_on32.png"
#endif


/** Default constructor */
MainWindow::MainWindow()
{
#if defined(Q_WS_MAC)
  _menuBar = 0;
#endif

  /* Set Vidalia's application icon */
  setWindowIcon(QIcon(IMG_TOR_STOPPED));
  QApplication::setWindowIcon(QIcon(IMG_TOR_STOPPED));

  /* Create the actions that will go in the tray menu */
  createActions();
  /* Create the tray menu itself */
  createMenus(); 
  /* Create the menubar (Mac) */
  createMenuBar();

  /* Create a new TorControl object, used to communicate with and manipulate Tor */
  _torControl = new TorControl();
  connect(_torControl, SIGNAL(started()), this, SLOT(started()));
  connect(_torControl, SIGNAL(stopped(int, QProcess::ExitStatus)),
                 this, SLOT(stopped(int, QProcess::ExitStatus)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(connected()));

  /* Create a new MessageLog object so messages can be logged when not shown */
  _messageLog = new MessageLog(_torControl, this);
 
  /* Create a new BandwidthGraph object so we can monitor bandwidth usage */
  Qt::WFlags bw_flags = (Qt::Tool | Qt::WindowStaysOnTopHint);
  _bandwidthGraph = new BandwidthGraph(this, bw_flags);
  
  /* Put an icon in the system tray to indicate the status of Tor */
  _trayIcon = new TrayIcon(QPixmap(IMG_TOR_STOPPED),
                           tr("Tor is Stopped"), _trayMenu, this);
  _trayIcon->show();
}

/** Default destructor */
MainWindow::~MainWindow()
{
  if (_torControl) {
    delete _torControl;
  }
  if (_messageLog) {
    delete _messageLog;
  }
  if (_bandwidthGraph) {
    delete _bandwidthGraph;
  }
}

/** Shows the menubar on Mac */
void
MainWindow::show()
{
  createMenuBar();
}

/** Called when the application is closing, by selecting "Exit" from the tray
 * menu. This function disconnects the control socket and ends the Tor
 * process. */
void
MainWindow::close()
{
  /* Disconnect all of the TorControl object's signals */
  disconnect(_torControl, 0, 0, 0);

  /* Close the control socket */
  if (_torControl->isConnected()) {
    _torControl->disconnect();
  }

  /* Stop the Tor process */
  if (_torControl->isRunning()) {
    _torControl->stop();
  }

  /* Remove the menu bar on Mac */
  removeMenuBar();
  
  /* And then quit for real */
  QCoreApplication::quit();
}

/* 
  Create and bind actions to events. Setup for initial
  tray menu configuration.
*/
void 
MainWindow::createActions()
{
  _startAct = new QAction(tr("Start"), this);
  connect(_startAct, SIGNAL(triggered()),
      this, SLOT(start()));
  _startAct->setEnabled(true);
  
  _stopAct = new QAction(tr("Stop"), this);
  connect(_stopAct, SIGNAL(triggered()),
      this, SLOT(stop()));
  _stopAct->setEnabled(false);

  _configAct = new QAction(tr("Configure"), this);
  
  _aboutAct = new QAction(tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()),
      this, SLOT(showAbout()));
  
  _exitAct = new QAction(tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()),
      this, SLOT(close()));

  _bandwidthAct = new QAction(tr("Bandwidth Graph"), this);
  connect(_bandwidthAct, SIGNAL(triggered()),
      this, SLOT(showBandwidthGraph()));

  _messageAct = new QAction(tr("Message Log"), this);
  connect(_messageAct, SIGNAL(triggered()),
      this, SLOT(showMessageLog()));
}

/*
  Creates a QMenu object that contains QActions
  which compose the system tray menu.
*/
void 
MainWindow::createMenus()
{
  /* Tray menu */ 
  _trayMenu = new QMenu(this);
  _trayMenu->addAction(_startAct);
  _trayMenu->addAction(_stopAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_bandwidthAct);
  _trayMenu->addAction(_messageAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_configAct);
  _trayMenu->addAction(_aboutAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_exitAct);
}

/** Creates a new menubar with no parent, so Qt will use this as the "default
 * menubar" on Mac. Note that to force this menubar to be displayed after all
 * child windows are closed (since we don't actually have a visible main
 * window), you must remove and re-add this no-parent menubar */
void
MainWindow::createMenuBar()
{
#if defined(Q_WS_MAC)
  /* Mac users sure like their shortcuts. Actions NOT mentioned below
   * don't explicitly need shortcuts, since they are merged to the default
   * menubar and get the default shortcuts anyway. */
  _startAct->setShortcut(tr("Ctrl+S"));
  _stopAct->setShortcut(tr("Ctrl+T"));
  _bandwidthAct->setShortcut(tr("Ctrl+B"));
  _messageAct->setShortcut(tr("Ctrl+L"));
  
  /* The File, Help, and Configure menus will get merged into the application
   * menu by Qt. */
   if (_menuBar) delete _menuBar;
  _menuBar = new QMenuBar();
  _fileMenu = _menuBar->addMenu(tr("File"));
  _fileMenu->addAction(_exitAct);
  _torMenu = _menuBar->addMenu(tr("Tor"));
  _torMenu->addAction(_startAct);
  _torMenu->addAction(_stopAct);
  _viewMenu = _menuBar->addMenu(tr("View"));
  _viewMenu->addAction(_bandwidthAct);
  _viewMenu->addAction(_messageAct);
  _viewMenu->addAction(_configAct);
  _helpMenu = _menuBar->addMenu(tr("Help"));
  _helpMenu->addAction(_aboutAct);
#endif
}

/** Removes the menubar. */
void
MainWindow::removeMenuBar()
{
#if defined(Q_WS_MAC)
  if (_menuBar) {
    delete _menuBar;
  }
#endif
}

/*
 Starts Tor, modifies tray icon and tray menu appropriately
*/
void 
MainWindow::start()
{
  QString errmsg;
  if (!_torControl->start(&errmsg)) {
    QMessageBox::warning(this, tr("Error Starting Tor"),
       tr("Vidalia was unable to start Tor.\n\nError: ") + errmsg,
       QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/** Slot: Called when the Tor process is started. It will connect the control
 * socket and set the icons and tooltips accordingly. */
void 
MainWindow::started()
{
  /* Set the window icon */
  QApplication::setWindowIcon(QIcon(IMG_TOR_RUNNING));
  
  /* Set correct tray icon and tooltip */
  _trayIcon->setIcon(QPixmap(IMG_TOR_RUNNING));
  _trayIcon->setToolTip(tr("Tor is started"));

  /* Set menu actions appropriately */
  _startAct->setEnabled(false);
  _stopAct->setEnabled(true);

  /* Try to connect to Tor's control port */
  QString errmsg;
  if (!_torControl->connect(&errmsg)) {
    /* There's a possibility that Tor has started, but it just hasn't had a
     * chance to bind the control socket yet. So pause for a second and try
     * again.*/
    v_sleep(1);
    if(!_torControl->connect(&errmsg)) {
      /* Ok, ok. It really isn't going to connect. I give up. */
      QMessageBox::warning(this, tr("Error Connecting to Tor"),
         tr("Tor started successfully, but Vidalia was unable to "
            "connect to it. Check your control port settings and try "
            "again.\n\n") + errmsg,
         QMessageBox::Ok, QMessageBox::NoButton);
    }
  } 
}

/*
 Stops Tor
*/
void 
MainWindow::stop()
{
  QString errmsg;
  
  /* Disconnect the controller. Note that we don't check any error messages or
   * return values, since we'll just be killing Tor anyway. */
  if (_torControl->isConnected()) {
    _torControl->disconnect();
  }

  /* Stop the Tor process */
  _isIntentionalExit = true;
  if (!_torControl->stop(&errmsg)) {
    QMessageBox::warning(this, tr("Error Stopping Tor"),
       tr("Vidalia was unable to stop Tor.\n\nError: ") + errmsg,
       QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/** Slot: Called when the Tor process has exited. It will adjust the tray
 * icons and tooltips accordingly. */
void 
MainWindow::stopped(int exitCode, QProcess::ExitStatus exitStatus)
{
  /* Set the window icon */
  QApplication::setWindowIcon(QIcon(IMG_TOR_STOPPED));
  
  /* Set correct tray icon and tooltip */
  _trayIcon->setIcon(QPixmap(IMG_TOR_STOPPED));
  _trayIcon->setToolTip(tr("Tor is stopped"));

  /* Set menu actions appropriately */
  _stopAct->setEnabled(false);
  _startAct->setEnabled(true);

  /* If we didn't intentionally close Tor, then check to see if it crashed or
   * if it closed itself and returned an error code. */
  if (!_isIntentionalExit) {
    if (exitStatus == QProcess::CrashExit) {
      QMessageBox::warning(this, tr("Tor Exited"),
         tr("Vidalia detected that Tor exited unexpectedly.\n\n"
            "Please check the message log for indicators\n"
            "about what happened to Tor before it exited."),
         QMessageBox::Ok, QMessageBox::NoButton);
    } else if (exitCode != 0) {
      /* A quick overview of Tor's code tells me that if it catches a SIGTERM or
      * SIGINT, Tor will exit(0). We might need to change this warning message
      * if this turns out to not be the case. */
      QMessageBox::warning(this, tr("Tor Exited"),
         tr("Tor exited and returned a non-zero exit code.\n\n"
            "Please check the message log for indicators\n"
            "about what happened to Tor before it exited."),
         QMessageBox::Ok, QMessageBox::NoButton);
    }
    
    /* Regardless of why it closed, it closed unintentionally so close the
     * control socket */
     _torControl->disconnect();
  }
  _isIntentionalExit = false;
}

/** Called when the control socket has successfully connected to Tor. */
void
MainWindow::connected()
{
  QString errmsg;

  /* The controller connected, so now send the AUTHENTICATE command */
  if (!_torControl->authenticate(&errmsg)) {
    QMessageBox::warning(this, tr("Authentication Error"),
      tr("Vidalia was unable to authenticate itself to Tor."
         "Check your authentication information and try again."
         "\n\nError: ") + errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
    _torControl->disconnect();
    return;
  }

  /* Register for interesting events */
  if (!_torControl->setEvents(&errmsg)) {
    QMessageBox::warning(this, tr("Error Setting Events"),
      tr("Vidalia was unable to register for events from Tor. "
         "Some features of Vidalia will be unavailable. "
         "\n\nError: ") + errmsg,
      QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/*
 Creates an instance of AboutDialog and shows it 
*/
void 
MainWindow::showAbout()
{
  static AboutDialog* aboutDialog = new AboutDialog(_torControl, this);
  if(!aboutDialog->isVisible()) {
    aboutDialog->show();
  } else {
    aboutDialog->activateWindow();
    aboutDialog->raise();
  }
}

/*
 Shows Message Log
*/
void
MainWindow::showMessageLog()
{
  if(!_messageLog->isVisible()) {
    _messageLog->show();
  } else {
    _messageLog->activateWindow();
    _messageLog->raise();
  }
}

/*
 Shows Bandwidth Graph
*/
void
MainWindow::showBandwidthGraph()
{
  if(!_bandwidthGraph->isVisible()) {
    _bandwidthGraph->show();
  } else {
    _bandwidthGraph->activateWindow();
    _bandwidthGraph->raise();
  }
}
