/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
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
 * \file mainwindow.cpp
 * \version $Id$
 * \brief Main (hidden) window. Creates tray menu and child windows
 *
 * Implements the main window. The main window is a hidden window that serves
 * as the parent of the tray icon and popup menu, as well as other application
 * dialogs.
 */

#include <QtGui>
#include <vidalia.h>
#include <util/html.h>

#include "common/vmessagebox.h"
#include "mainwindow.h"


#define IMG_APP_ICON       ":/images/16x16/tor-logo.png"
#define IMG_START          ":/images/16x16/tor-on.png"
#define IMG_STOP           ":/images/16x16/tor-off.png"
#define IMG_BWGRAPH        ":/images/16x16/utilities-system-monitor.png"
#define IMG_MESSAGELOG     ":/images/16x16/format-justify-fill.png"
#define IMG_CONFIG         ":/images/16x16/preferences-system.png"
#define IMG_IDENTITY       ":/images/16x16/system-users.png"
#define IMG_HELP           ":/images/16x16/help-browser.png"
#define IMG_ABOUT          ":/images/16x16/tor-logo.png"
#define IMG_EXIT           ":/images/16x16/emblem-unreadable.png"
#define IMG_NETWORK        ":/images/16x16/applications-internet.png"

/* Decide which of our four sets of tray icons to use. */
#if defined(USE_QSYSTEMTRAYICON)
/* QSystemTrayIcon is available */
#if defined(Q_WS_WIN)
/* QSystemTrayIcon on Windows wants 16x16 .png files */
#define IMG_TOR_STOPPED  ":/images/16x16/tor-off.png"
#define IMG_TOR_RUNNING  ":/images/16x16/tor-on.png"
#define IMG_TOR_STARTING ":/images/16x16/tor-starting.png"
#define IMG_TOR_STOPPING ":/images/16x16/tor-stopping.png"
#endif
#else
/* No QSystemTrayIcon support */
#if defined(Q_WS_WIN)
/* Use the .ico files */
#include "res/vidalia_win.rc.h"
#define IMG_TOR_STOPPED    QString::number(IDI_TOR_OFF)
#define IMG_TOR_RUNNING    QString::number(IDI_TOR_ON)
#define IMG_TOR_STARTING   QString::number(IDI_TOR_STARTING)
#define IMG_TOR_STOPPING   QString::number(IDI_TOR_STOPPING)
#endif
#endif
#if defined(Q_WS_MAC)
/* On Mac, we always go straight to Carbon to load our dock images 
 * from .icns files */
#define IMG_TOR_STOPPED    "tor-off"
#define IMG_TOR_RUNNING    "tor-on"
#define IMG_TOR_STARTING   "tor-starting"
#define IMG_TOR_STOPPING   "tor-stopping"
#elif defined(Q_WS_X11)
/* On X11, we just use always the 22x22 .png files */
#define IMG_TOR_STOPPED    ":/images/22x22/tor-off.png"
#define IMG_TOR_RUNNING    ":/images/22x22/tor-on.png"
#define IMG_TOR_STARTING   ":/images/22x22/tor-starting.png"
#define IMG_TOR_STOPPING   ":/images/22x22/tor-stopping.png"
#endif


/** Default constructor. It installs an icon in the system tray area and
 * creates the popup menu associated with that icon. */
MainWindow::MainWindow()
{
  VidaliaSettings settings;
  
  /* Set Vidalia's application icon */
  setWindowIcon(QIcon(IMG_APP_ICON));

  /* Create all the dialogs of which we only want one instance */
  _aboutDialog = new AboutDialog();
  _messageLog = new MessageLog();
  _bandwidthGraph = new BandwidthGraph();
  _netViewer = new NetViewer();
  _configDialog = new ConfigDialog();
  
  /* Create the actions that will go in the tray menu */
  createActions();
  /* Creates a tray icon with a context menu and adds it to the system's
   * notification area. */
  createTrayIcon();
  
  /* Create a new TorControl object, used to communicate with and manipulate Tor */
  _torControl = Vidalia::torControl(); 
  connect(_torControl, SIGNAL(started()), this, SLOT(started()));
  connect(_torControl, SIGNAL(startFailed(QString)),
                 this,   SLOT(startFailed(QString)));
  connect(_torControl, SIGNAL(stopped(int, QProcess::ExitStatus)),
                 this,   SLOT(stopped(int, QProcess::ExitStatus)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(connected()));
  connect(_torControl, SIGNAL(disconnected()), this, SLOT(disconnected()));
  connect(_torControl, SIGNAL(connectFailed(QString)), 
                 this,   SLOT(connectFailed(QString)));

  /* Make sure we shut down when the operating system is restarting */
  connect(vApp, SIGNAL(shutdown()), this, SLOT(shutdown()));

  if (_torControl->isRunning()) {
    /* Tor was already running */
    this->started();
  } else if (settings.runTorAtStart()) {
    /* If we're supposed to start Tor when Vidalia starts, then do it now */
    start();
  }
}

/** Destructor. */
MainWindow::~MainWindow()
{
  _trayIcon.hide();
  delete _messageLog;
  delete _netViewer;
  delete _bandwidthGraph;
  delete _aboutDialog;
  delete _configDialog;
}

/** Terminate the Tor process if it is being run under Vidalia, disconnect all
 * TorControl signals, and exit Vidalia. */
void
MainWindow::shutdown()
{
  if (_torControl->isVidaliaRunningTor()) {
    /* Kill our Tor process now */ 
    _torControl->stop();
  }

  /* Disconnect all of the TorControl object's signals */
  disconnect(_torControl, 0, 0, 0);

  /* And then quit for real */
  QCoreApplication::quit();
}

/** Called when the application is closing, by selecting "Exit" from the tray
 * menu. If we're running a Tor server, then ask if we want to kill Tor now,
 * or do a delayed shutdown. */
void
MainWindow::close()
{
  if (_torControl->isVidaliaRunningTor()) {
    /* If we're running a server currently, ask if we want to do a delayed
     * shutdown. If we do, then close Vidalia only when Tor stops. Otherwise,
     * kill Tor and bail now. */
    ServerSettings settings(_torControl);
    if (_torControl->isConnected() && settings.isServerEnabled()) {
      connect(_torControl, SIGNAL(stopped()), this, SLOT(shutdown()));
      if (!stop())
        disconnect(_torControl, SIGNAL(stopped()), this, SLOT(shutdown()));
      return;
    }
  }
  /* Shut down Tor (if necessary) and exit Vidalia */
  shutdown();
}

/** Create and bind actions to events. Setup for initial
 * tray menu configuration. */
void 
MainWindow::createActions()
{
  _startAct = new QAction(QIcon(IMG_START), tr("Start"), this);
  connect(_startAct, SIGNAL(triggered()), this, SLOT(start()));
  _startAct->setEnabled(true);
  
  _stopAct = new QAction(QIcon(IMG_STOP), tr("Stop"), this);
  connect(_stopAct, SIGNAL(triggered()), this, SLOT(stop()));
  _stopAct->setEnabled(false);

  _exitAct = new QAction(QIcon(IMG_EXIT), tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _bandwidthAct = new QAction(QIcon(IMG_BWGRAPH), tr("Bandwidth Graph"), this);
  connect(_bandwidthAct, SIGNAL(triggered()), 
          _bandwidthGraph, SLOT(showWindow()));

  _messageAct = new QAction(QIcon(IMG_MESSAGELOG), tr("Message Log"), this);
  connect(_messageAct, SIGNAL(triggered()),
          _messageLog, SLOT(showWindow()));

  _networkAct = new QAction(QIcon(IMG_NETWORK), tr("Network Map"), this);
  connect(_networkAct, SIGNAL(triggered()), 
          _netViewer, SLOT(showWindow()));
  
  _configAct = new QAction(QIcon(IMG_CONFIG), tr("Settings"), this);
  connect(_configAct, SIGNAL(triggered()), 
          _configDialog, SLOT(showWindow()));
  
  _aboutAct = new QAction(QIcon(IMG_ABOUT), tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()), 
          _aboutDialog, SLOT(showWindow()));

  _helpAct = new QAction(QIcon(IMG_HELP), tr("Help"), this);
  connect(_helpAct, SIGNAL(triggered()), vApp, SLOT(help()));
  
  _newIdentityAct = new QAction(QIcon(IMG_IDENTITY), tr("New Identity"), this);
  _newIdentityAct->setEnabled(false);
  connect(_newIdentityAct, SIGNAL(triggered()), this, SLOT(newIdentity()));
}

/** Creates a tray icon with a context menu and adds it to the system
 * notification area. On Mac, we also set up an application menubar. */
void
MainWindow::createTrayIcon()
{
  /* We start with an icon that says Tor is not running */
  updateTrayIcon(IMG_TOR_STOPPED, tr("Tor is Stopped"));
  /* Create the default menu bar (Mac) */
  createMenuBar();
  /* Create a tray menu and add it to the tray icon */
  _trayIcon.setContextMenu(createTrayMenu());
  /* Make the tray icon visible */
  _trayIcon.show();
}

/** Creates a QMenu object that contains QActions which compose the system 
 * tray menu. */
QMenu* 
MainWindow::createTrayMenu()
{
  QMenu *menu = new QMenu(this);
  menu->addAction(_startAct);
  menu->addAction(_stopAct);
  menu->addSeparator();
  menu->addAction(_bandwidthAct);
  menu->addAction(_messageAct);
  menu->addAction(_networkAct);
  menu->addAction(_newIdentityAct);
  
#if !defined(Q_WS_MAC)
  /* These aren't added to the dock menu on Mac, since they are in the
   * standard Mac locations in the menu bar. */
  menu->addSeparator();
  menu->addAction(_configAct);
  menu->addAction(_helpAct);
  menu->addAction(_aboutAct);
  menu->addSeparator();
  menu->addAction(_exitAct);
#endif
  return menu;
}

/** Creates a new menubar with no parent, so Qt will use this as the "default
 * menubar" on Mac. This adds on to the existing actions from the createMens()
 * method. */
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
  _networkAct->setShortcut(tr("Ctrl+N"));
  _helpAct->setShortcut(tr("Ctrl+?"));
  _newIdentityAct->setShortcut(tr("Ctrl+I"));

  /* Force Qt to put merge the Exit, Configure, and About menubar options into
   * the default menu, even if Vidalia is currently not speaking English. */
  _exitAct->setText("exit");
  _configAct->setText("config");
  _aboutAct->setText("about");
  
  /* The File, Help, and Configure menus will get merged into the application
   * menu by Qt. */
  QMenuBar *menuBar = new QMenuBar();
  QMenu *fileMenu = menuBar->addMenu(tr("File"));
  fileMenu->addAction(_exitAct);
  
  QMenu *torMenu = menuBar->addMenu(tr("Tor"));
  torMenu->addAction(_startAct);
  torMenu->addAction(_stopAct);
  torMenu->addSeparator();
  torMenu->addAction(_newIdentityAct);

  QMenu *viewMenu = menuBar->addMenu(tr("View"));
  viewMenu->addAction(_bandwidthAct);
  viewMenu->addAction(_messageAct);
  viewMenu->addAction(_networkAct);
  viewMenu->addAction(_configAct);
  
  QMenu *helpMenu = menuBar->addMenu(tr("Help"));
  _helpAct->setText(tr("Vidalia Help"));
  helpMenu->addAction(_helpAct);
  helpMenu->addAction(_aboutAct);
#endif
}

/** Sets the tray icon's image and tooltip. */
void
MainWindow::updateTrayIcon(QString iconFile, QString tooltip)
{
  /* Set the tray icon's image */
#if defined(USE_QSYSTEMTRAYICON)
  _trayIcon.setIcon(QIcon(iconFile));
#else
  _trayIcon.setIcon(iconFile);
#endif
  /* Set the icon's tooltip */
  _trayIcon.setToolTip(tooltip);
}

/** Attempts to start Tor. If Tor fails to start, then startFailed() will be
 * called with an error message containing the reason. */
void 
MainWindow::start()
{
  /* This doesn't get set to false until Tor is actually up and running, so we
   * don't yell at users twice if their Tor doesn't even start, due to the fact
   * that QProcess::stopped() is emitted even if the process didn't even
   * start. */
  _isIntentionalExit = true;
  /* Kick off the Tor process */
  _torControl->start();
  /* Don't let the user start Tor twice. That would be silly. */
  _startAct->setEnabled(false);
}

/** Called when the Tor process fails to start, for example, because the path
 * specified to the Tor executable didn't lead to an executable. */
void
MainWindow::startFailed(QString errmsg)
{
  /* We don't display the error message for now, because the error message
   * that Qt gives us in this instance is almost always "Unknown Error". That
   * will make users sad. */
  Q_UNUSED(errmsg);
  
  /* Display an error message and see if the user wants some help */
  int response = VMessageBox::warning(this, tr("Error Starting Tor"),
                   tr("Vidalia was unable to start Tor. Check your settings "
                        "to ensure the correct name and location of your Tor "
                        "executable is specified."),
                   VMessageBox::Ok, VMessageBox::ShowSettings, VMessageBox::Help);

  if (response == VMessageBox::ShowSettings) {
    /* Show the settings dialog so the user can make sure they're pointing to
     * the correct Tor. */
     ConfigDialog* configDialog = new ConfigDialog(this);
     configDialog->showWindow(ConfigDialog::General);
  } else if (response == VMessageBox::Help) {
    /* Show troubleshooting information about starting Tor */
    Vidalia::help("troubleshooting.start");
  }
  _startAct->setEnabled(true);
}

/** Slot: Called when the Tor process is started. It will connect the control
 * socket and set the icons and tooltips accordingly. */
void 
MainWindow::started()
{
  /* Now that Tor is running, we want to know if it dies when we didn't want
   * it to. */
  _isIntentionalExit = false;
  /* We haven't started a delayed shutdown yet. */
  _delayedShutdownStarted = false;
  /* Set correct tray icon and tooltip */
  updateTrayIcon(IMG_TOR_STARTING, tr("Tor is starting"));
  /* Set menu actions appropriately */
  _stopAct->setEnabled(true);
  _startAct->setEnabled(false);
  /* Try to connect to Tor's control port */
  _torControl->connect();
}

/** Called when the connection to the control socket fails. The reason will be
 * given in the errmsg parameter. */
void
MainWindow::connectFailed(QString errmsg)
{
  /* Ok, ok. It really isn't going to connect. I give up. */
  int response = VMessageBox::warning(this, 
                   tr("Error Connecting to Tor"), p(errmsg),
                   VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape, 
                   VMessageBox::Retry, VMessageBox::Help);


  if (response == VMessageBox::Retry) {
    /* Let's give it another try. */
    _torControl->connect();
  } else {
    /* Show the help browser (if requested) */
    if (response == VMessageBox::Help) {
      Vidalia::help("troubleshooting.connect");
    }
    /* Since Vidalia can't connect, we can't really do much, so stop Tor. */
    _torControl->stop();
  }
}

/** Disconnects the control socket and stops the Tor process. */
bool
MainWindow::stop()
{
  ServerSettings server(_torControl);
  QString errmsg;
  bool rc, delayShutdown = false;
  
  /* If we're running a server, give users the option of terminating
   * gracefully so clients have time to find new servers. */
  if (server.isServerEnabled() && !_delayedShutdownStarted) {
    /* Ask the user if they want to shutdown nicely. */
    int response = VMessageBox::question(this, tr("Server is Enabled"),
                     tr("You are currently running a Tor server. "
                        "Terminating your server will interrupt any "
                        "open connections from clients.\n\n"
                        "Would you like to shutdown gracefully and "
                        "give clients time to find a new server?"),
                        VMessageBox::Yes|VMessageBox::Default, 
                        VMessageBox::No, 
                        VMessageBox::Cancel|VMessageBox::Escape);
    if (response == VMessageBox::Yes)
      delayShutdown = true;
    else if (response == VMessageBox::Cancel)
      return false;
  }

  if (delayShutdown) {
    /* Start a delayed shutdown */
    rc = _torControl->signal(TorSignal::Shutdown, &errmsg);
    _delayedShutdownStarted = rc;
  } else {
    /* We want Tor to stop now, regardless of whether we're a server. */
    _isIntentionalExit = true;
    if ((rc = _torControl->stop(&errmsg)) == true)
      _stopAct->setEnabled(false);
  }
  
  if (rc) {
    /* Indicate that Tor is about to shut down */
    updateTrayIcon(IMG_TOR_STOPPING, tr("Tor is stopping"));
  } else {
    /* We couldn't tell Tor to stop, for some reason. */
    int response = VMessageBox::warning(this, tr("Error Stopping Tor"),
                     p(tr("Vidalia was unable to stop Tor.")) + p(errmsg),
                     VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape, 
                     VMessageBox::Help);
      
    if (response == VMessageBox::Help) {
      /* Show some troubleshooting help */
      Vidalia::help("troubleshooting.stop");
    }
    /* Tor is still running since stopping failed */
    _isIntentionalExit = false;
  }
  return rc;
}

/** Slot: Called when the Tor process has exited. It will adjust the tray
 * icons and tooltips accordingly. */
void 
MainWindow::stopped(int exitCode, QProcess::ExitStatus exitStatus)
{
  /* Set correct tray icon and tooltip */
  updateTrayIcon(IMG_TOR_STOPPED, tr("Tor is stopped"));
  /* Set menu actions appropriately */
  _startAct->setEnabled(true);

  /* If we didn't intentionally close Tor, then check to see if it crashed or
   * if it closed itself and returned an error code. */
  if (!_isIntentionalExit) {
    _stopAct->setEnabled(false);
    /* A quick overview of Tor's code tells me that if it catches a SIGTERM or
     * SIGINT, Tor will exit(0). We might need to change this warning message
     * if this turns out to not be the case. */
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
      int ret = VMessageBox::warning(this, tr("Tor Exited"),
                  tr("Vidalia detected that Tor exited unexpectedly.\n\n"
                     "Please check the message log for indicators "
                     "about what happened to Tor before it exited."),
                  VMessageBox::Ok, VMessageBox::ShowLog, VMessageBox::Help);
      if (ret == VMessageBox::ShowLog) {
        _messageLog->showWindow();  
      } else if (ret == VMessageBox::Help) {
        Vidalia::help("troubleshooting.torexited");
      }
    }
  }
}

/** Called when the control socket has successfully connected to Tor. */
void
MainWindow::connected()
{
  ServerSettings serverSettings(_torControl);
  QString errmsg;

  /* Update our tray status icon */
  updateTrayIcon(IMG_TOR_RUNNING, tr("Tor is running"));
  _newIdentityAct->setEnabled(true);

  /* If the user changed some of the server's settings while Tor wasn't 
   * running, then we better let Tor know about the changes now. */
  if (serverSettings.changedSinceLastApply()) {
    if (!serverSettings.apply(&errmsg)) {
      int ret = VMessageBox::warning(this, 
                  tr("Error Applying Server Settings"),
                  p(tr("Vidalia was unable to apply your server's settings."))
                    + p(errmsg),
                  VMessageBox::Ok, VMessageBox::ShowSettings, VMessageBox::ShowLog);

      if (ret == VMessageBox::ShowSettings) {
        /* Show the config dialog with the server page already shown. */
        _configDialog->showWindow(ConfigDialog::Server);
      } else if (ret == VMessageBox::ShowLog) {
        /* Show the message log. */
        _messageLog->showWindow(); 
      }
    }
  }
}

/** Called when the control socket has been disconnected. */
void
MainWindow::disconnected()
{
  _newIdentityAct->setEnabled(false);
}

/** Called when the user selects the "New Identity" action from the menu. */
void
MainWindow::newIdentity()
{
  QString errmsg;

  /* Send the NEWNYM signal. If message balloons are supported and the NEWNYM
   * is successful, we will show the result as a balloon. Otherwise, we'll 
   * just use a message box. */
  if (_torControl->signal(TorSignal::NewNym, &errmsg)) {
    /* NEWNYM signal was successful */
    QString title = tr("New Identity");
    QString message = tr("All subsequent connections will "
                         "appear to be different than your "
                         "old connections.");
#if defined(USE_QSYSTEMTRAYICON)
    if (QSystemTrayIcon::supportsMessages()) {
      _trayIcon.showMessage(title, message, QSystemTrayIcon::Information);
    } else {
      VMessageBox::information(this, title, message, VMessageBox::Ok);
    }
#else
    VMessageBox::information(this, title, message, VMessageBox::Ok);
#endif
  } else {
    /* NEWNYM signal failed */
    VMessageBox::warning(this, 
      tr("Failed to Create New Identity"), errmsg, VMessageBox::Ok);
  }
}

