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
 * \file mainwindow.cpp
 * \version $Id$
 *
 * Implements the main window. The main window is a hidden window that serves
 * as the parent of the tray icon and popup menu, as well as other application
 * dialogs.
 */

#include <QtGui>

#include <vidalia.h>
#include <util/html.h>
#include <config/messagetypes.h>

#include "mainwindow.h"

#define IMG_APP_ICON       ":/images/16x16/tor-logo.png"
#define IMG_START          ":/images/16x16/tor-on.png"
#define IMG_STOP           ":/images/16x16/tor-off.png"
#define IMG_BWGRAPH        ":/images/16x16/utilities-system-monitor.png"
#define IMG_MESSAGELOG     ":/images/16x16/format-justify-fill.png"
#define IMG_CONFIG         ":/images/16x16/preferences-system.png"
#define IMG_HELP           ":/images/16x16/help-browser.png"
#define IMG_ABOUT          ":/images/16x16/tor-logo.png"
#define IMG_EXIT           ":/images/16x16/emblem-unreadable.png"
#define IMG_NETWORK        ":/images/16x16/applications-internet.png"

#if defined(Q_WS_MAC)
/* On Mac, we go straight to Carbon to load our dock images from .icns files */
#define IMG_TOR_STOPPED    "tor-off"
#define IMG_TOR_RUNNING    "tor-on"
#define IMG_TOR_STARTING   "tor-starting"
#define IMG_TOR_STOPPING   "tor-stopping"
#elif defined(Q_WS_X11)
/* On X11, we just use the .png files */
#define IMG_TOR_STOPPED    ":/images/22x22/tor-off.png"
#define IMG_TOR_RUNNING    ":/images/22x22/tor-on.png"
#define IMG_TOR_STARTING   ":/images/22x22/tor-starting.png"
#define IMG_TOR_STOPPING   ":/images/22x22/tor-stopping.png"
#else
/* On Win32, we load .ico files so our icons look correct on all Windowses */
#include "res/vidalia_win.rc.h"
#define IMG_TOR_STOPPED    QString::number(IDI_TOR_OFF)
#define IMG_TOR_RUNNING    QString::number(IDI_TOR_ON)
#define IMG_TOR_STARTING   QString::number(IDI_TOR_STARTING)
#define IMG_TOR_STOPPING   QString::number(IDI_TOR_STOPPING)
#endif

/* Buttons the use can click in error message boxes */
#define BUTTON_OK         0
#define BUTTON_RETRY      1
#define BUTTON_HELP       2


/** Default constructor. It installs an icon in the system tray area and
 * creates the popup menu associated with that icon. */
MainWindow::MainWindow()
{
  VidaliaSettings settings;
  
  /* Set Vidalia's application icon */
  setWindowIcon(QIcon(IMG_APP_ICON));

  /* Create the actions that will go in the tray menu */
  createActions();
  /* Create the tray menu itself */
  createMenus(); 

  /* Create a new TorControl object, used to communicate with and manipulate Tor */
  _torControl = Vidalia::torControl(); 
  connect(_torControl, SIGNAL(started()), this, SLOT(started()));
  connect(_torControl, SIGNAL(stopped(int, QProcess::ExitStatus)),
                 this, SLOT(stopped(int, QProcess::ExitStatus)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(connected()));
  connect(_torControl, SIGNAL(connectFailed(QString)), 
                 this, SLOT(connectFailed(QString)));

  /* Create a new MessageLog object so messages can be logged when not shown */
  _messageLog = new MessageLog(this);
  
  /* Create a new BandwidthGraph object so we can monitor bandwidth usage */
  Qt::WFlags bw_flags = (Qt::Tool | Qt::WindowStaysOnTopHint);
  _bandwidthGraph = new BandwidthGraph(this, bw_flags);

  /* Create a new NetViewer object so we can monitor the network */
  //_netViewer = new NetViewer(this);

  /* Put an icon in the system tray to indicate the status of Tor */
  _trayIcon = new TrayIcon(IMG_TOR_STOPPED,
                           tr("Tor is Stopped"), _trayMenu);
  _trayIcon->show();
  
  /* If we're supposed to start Tor when Vidalia starts, then do it now */
  if (settings.runTorAtStart()) {
    start();
  }
}

/** Destructor. */
MainWindow::~MainWindow()
{
  _trayIcon->hide();
  delete _trayIcon;
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

  /* And then quit for real */
  QCoreApplication::quit();
}

/** Create and bind actions to events. Setup for initial
 * tray menu configuration. */
void 
MainWindow::createActions()
{
  _startAct = new QAction(QIcon(IMG_START), tr("Start"), this);
  connect(_startAct, SIGNAL(triggered()),
      this, SLOT(start()));
  _startAct->setEnabled(true);
  
  _stopAct = new QAction(QIcon(IMG_STOP), tr("Stop"), this);
  connect(_stopAct, SIGNAL(triggered()),
      this, SLOT(stop()));
  _stopAct->setEnabled(false);

  _configAct = new QAction(QIcon(IMG_CONFIG), tr("Configure"), this);
  connect(_configAct, SIGNAL(triggered()),
      this, SLOT(showConfig()));
  
  _aboutAct = new QAction(QIcon(IMG_ABOUT), tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()),
      this, SLOT(showAbout()));
  
  _exitAct = new QAction(QIcon(IMG_EXIT), tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()),
      this, SLOT(close()));

  _bandwidthAct = new QAction(QIcon(IMG_BWGRAPH), tr("Bandwidth Graph"), this);
  connect(_bandwidthAct, SIGNAL(triggered()),
      this, SLOT(showBandwidthGraph()));

  _messageAct = new QAction(QIcon(IMG_MESSAGELOG), tr("Message Log"), this);
  connect(_messageAct, SIGNAL(triggered()),
      this, SLOT(showMessageLog()));

  _helpAct = new QAction(QIcon(IMG_HELP), tr("Help"), this);
  connect(_helpAct, SIGNAL(triggered()), 
      this, SLOT(showHelp()));
/*
  _networkAct = new QAction(QIcon(IMG_NETWORK), tr("View Network"), this);
  connect(_networkAct, SIGNAL(triggered()),
      this, SLOT(showNetwork()));
*/
}

/**
 * Creates a QMenu object that contains QActions
 *  which compose the system tray menu.
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
  //_trayMenu->addAction(_networkAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_configAct);
  _trayMenu->addAction(_helpAct);
  _trayMenu->addAction(_aboutAct);
  _trayMenu->addSeparator();
  _trayMenu->addAction(_exitAct);

  createMenuBar();
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
  //_networkAct->setShortcut(tr("Ctrl+N"));
  _helpAct->setShortcut(tr("Ctrl+?"));

  /* The File, Help, and Configure menus will get merged into the application
   * menu by Qt. */
  QMenuBar *menuBar = new QMenuBar();
  QMenu *fileMenu = menuBar->addMenu(tr("File"));
  fileMenu->addAction(_exitAct);
  
  QMenu *torMenu = menuBar->addMenu(tr("Tor"));
  torMenu->addAction(_startAct);
  torMenu->addAction(_stopAct);
  
  QMenu *viewMenu = menuBar->addMenu(tr("View"));
  viewMenu->addAction(_bandwidthAct);
  viewMenu->addAction(_messageAct);
//  viewMenu->addAction(_networkAct);
  viewMenu->addAction(_configAct);
  
  QMenu *helpMenu = menuBar->addMenu(tr("Help"));
  _helpAct->setText(tr("Vidalia Help"));
  helpMenu->addAction(_helpAct);
  helpMenu->addAction(_aboutAct);
#endif
}

/** Attempts to start Tor. If starting Tor fails, then it will display a
 * message box giving the user an error message regarding why Tor failed to
 * start.
 * \sa started()
 */
void 
MainWindow::start()
{
  QString errmsg;
  _isIntentionalExit = false;
  if (_torControl->start(&errmsg)) {
    _startAct->setEnabled(false);
  } else {
    /* Display an error message and see if the user wants some help */
    int response = QMessageBox::warning(this, tr("Error Starting Tor"),
                     p(tr("Vidalia was unable to start Tor.")) + p(errmsg),
                     tr("OK"), tr("Help"));

    if (response == 1) {
      /* Show troubleshooting information about starting Tor */
      Vidalia::help("troubleshooting.start");
    }
  }
}

/** Slot: Called when the Tor process is started. It will connect the control
 * socket and set the icons and tooltips accordingly. */
void 
MainWindow::started()
{
  QString errmsg;
  
  /* Set correct tray icon and tooltip */
  _trayIcon->update(IMG_TOR_STARTING, tr("Tor is starting"));

  /* Set menu actions appropriately */
  _stopAct->setEnabled(true);

  /* Try to connect to Tor's control port */
  _torControl->connect();
}

/** Called when the connection to the control socket fails. The reason will be
 * given in the errmsg parameter. */
void
MainWindow::connectFailed(QString errmsg)
{
  /* Ok, ok. It really isn't going to connect. I give up. */
  int response = QMessageBox::warning(this, 
                   tr("Error Connecting to Tor"), p(errmsg),
                   tr("OK"), tr("Retry"), tr("Help"),
                   BUTTON_OK, 0);


  if (response == BUTTON_RETRY) {
    _torControl->connect();
  } else {
    /* Show the help browser (if requested) */
    if (response == BUTTON_HELP) {
      Vidalia::help("troubleshooting.connect");
    }
    /* Since Vidalia can't connect, we can't really do much, so stop Tor. */
    _torControl->stop();
  }
}

/** Gives users the option of shutting down a server gracefully, giving
 * clients time to find a new circuit. Returns true if the timed server
 * shutdown was initiated successfully or false if we want to terminate
 * forcefully. */
bool
MainWindow::initiateServerShutdown()
{
  QString errmsg;
  bool rc = false;
  
  /* Ask the user if they want to shutdown nicely. */
  int response = QMessageBox::question(this, tr("Server is Enabled"),
                   p(tr("You are currently running a Tor server.\n"
                        "Terminating your server will interrupt any "
                        "open connections from clients.\n\n"
                        "Would you like to shutdown gracefully and "
                        "give clients time to find a new server?")),
                   QMessageBox::Yes, QMessageBox::No);

  if (response == QMessageBox::Yes) {
    /* Send a SHUTDOWN signal to Tor */
    if (_torControl->signal(TorSignal::Shutdown, &errmsg)) {
      rc = true; /* Shutdown successfully initiated */
    } else {
      /* Let the user know that we couldn't shutdown gracefully and we'll
       * kill Tor forcefully now if they want. */
      response = QMessageBox::warning(this, tr("Error Shutting Down"),
                  p(tr("Vidalia was unable to shutdown Tor gracefully.\n") 
                   + errmsg) + p(tr("Do you want to close Tor anyway?")),
                  QMessageBox::Yes, QMessageBox::No);

      if (response == QMessageBox::No) {
        /* Don't try to terminate Tor anymore. Just leave it running */
        _trayIcon->update(IMG_TOR_RUNNING, tr("Tor is running"));
        rc = true;
      }
    }
  }
  return rc;
}

/** Disconnects the control socket and stops the Tor process. */
void 
MainWindow::stop()
{
  ServerSettings server(_torControl);
  QString errmsg;

  /* Indicate that Tor is about to shut down */
  _trayIcon->update(IMG_TOR_STOPPING, tr("Tor is stopping"));

  /* If we're running a server, give users the option of terminating
   * gracefully so clients have time to find new servers. */
  if (server.isServerEnabled()) {
    if (initiateServerShutdown()) {
      /* Server shutdown was started successfully. */
      return;
    }
  }

  /* Terminate the Tor process immediately */
  _isIntentionalExit = true;
  if (_torControl->stop(&errmsg)) {
    _stopAct->setEnabled(false);
  } else {
    int response = QMessageBox::warning(this, tr("Error Stopping Tor"),
                     p(tr("Vidalia was unable to stop Tor.")) + p(errmsg),
                     tr("OK"), tr("Help"),
                     BUTTON_OK, BUTTON_OK);
      
    if (response == BUTTON_HELP) {
      /* Show some troubleshooting help */
      Vidalia::help("troubleshooting.stop");
    }
    
    /* Tor is still running since stopping failed */
    _trayIcon->update(IMG_TOR_RUNNING, tr("Tor is running"));
    _isIntentionalExit = false;
  }
}

/** Slot: Called when the Tor process has exited. It will adjust the tray
 * icons and tooltips accordingly. */
void 
MainWindow::stopped(int exitCode, QProcess::ExitStatus exitStatus)
{
  /* Set correct tray icon and tooltip */
  _trayIcon->update(IMG_TOR_STOPPED, tr("Tor is stopped"));

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
      int ret = QMessageBox::warning(this, tr("Tor Exited"),
                  p(tr("Vidalia detected that Tor exited unexpectedly.\n\n"
                       "Please check the message log for indicators\n"
                       "about what happened to Tor before it exited.")),
                  tr("Show Log"), tr("Close"));
      if (ret == 0) {
        showMessageLog();  
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
  _trayIcon->update(IMG_TOR_RUNNING, tr("Tor is running"));
  
  /* If the user changed some of the server's settings while Tor wasn't 
   * running, then we better let Tor know about the changes now. */
  if (serverSettings.changedSinceLastApply()) {
    if (!serverSettings.apply(&errmsg)) {
      int ret = QMessageBox::warning(this, 
                  tr("Error Applying Server Settings"),
                  p(tr("Vidalia was unable to apply your server's settings."))
                    + p(errmsg),
                  tr("OK"), tr("Show Settings"), tr("Show Log"));

      if (ret == 1) {
        /* Show the config dialog with the server page already shown. */
        ConfigDialog* configDialog = new ConfigDialog(this);
        configDialog->show(ConfigDialog::Server);
      } else if (ret == 2) {
        /* Show the message log. */
        showMessageLog(); 
      }
    }
  }
}

/** Creates an instance of AboutDialog and shows it. If the About dialog is
 * already displayed, the existing instance will be brought to the foreground. */
void 
MainWindow::showAbout()
{
  static AboutDialog* aboutDialog = new AboutDialog(this);
  aboutDialog->show();
}

/** Shows Message Log. If the message log is already displayed, the existing
 * instance will be brought to the foreground. */
void
MainWindow::showMessageLog()
{
  _messageLog->show();
}

/** Shows Bandwidth Graph. If the bandwidth graph is already displayed, the
 * existing instance will be brought to the foreground. */
void
MainWindow::showBandwidthGraph()
{
  _bandwidthGraph->show();
}

/** Shows Configuration dialog. If the config dialog is already displayed, the
 * existing instance will be brought to the foreground. */
void
MainWindow::showConfig()
{
  static ConfigDialog* configDialog = new ConfigDialog(this);
  configDialog->show();
}

/** Shows Help Browser. If the browser is already displayed, the existing
 * instance will be brought to the foreground. */
void
MainWindow::showHelp()
{
  Vidalia::help(); 
}

/** Shows the View Network dialog. If the View Network dialog is already
 *  displayed, the existing instance will be brought to the foreground. */
void
MainWindow::showNetwork()
{
  _netViewer->show();  
}

