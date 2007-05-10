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
#include <QTimer>
#include <vidalia.h>
#include <util/html.h>

#include "common/vmessagebox.h"
#include "common/animatedpixmap.h"
#include "mainwindow.h"


#define IMG_APP_ICON       ":/images/16x16/tor-logo.png"
#define IMG_BWGRAPH        ":/images/16x16/utilities-system-monitor.png"
#define IMG_CONTROL_PANEL  ":/images/16x16/preferences-desktop.png"
#define IMG_MESSAGELOG     ":/images/16x16/format-justify-fill.png"
#define IMG_CONFIG         ":/images/16x16/preferences-system.png"
#define IMG_IDENTITY       ":/images/16x16/system-users.png"
#define IMG_HELP           ":/images/16x16/help-browser.png"
#define IMG_ABOUT          ":/images/16x16/tor-logo.png"
#define IMG_EXIT           ":/images/16x16/emblem-unreadable.png"
#define IMG_NETWORK        ":/images/16x16/applications-internet.png"

#define IMG_START_TOR_16     ":/images/16x16/start-tor.png"
#define IMG_STOP_TOR_16      ":/images/16x16/stop-tor.png"
#define IMG_START_TOR_48     ":/images/48x48/start-tor.png"
#define IMG_STOP_TOR_48      ":/images/48x48/stop-tor.png"
#define IMG_TOR_STOPPED_48   ":/images/48x48/tor-off.png"
#define IMG_TOR_RUNNING_48   ":/images/48x48/tor-on.png"
#define IMG_TOR_STARTING_48  ":/images/48x48/tor-starting.png"
#define IMG_TOR_STOPPING_48  ":/images/48x48/tor-stopping.png"

#define ANIM_PROCESS_WORKING  ":/images/32x32/process-working.png"

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

/** Only allow 'New Identity' to be clicked once every 60 seconds. */
#define MIN_NEWIDENTITY_INTERVAL   (60*1000)


/** Default constructor. It installs an icon in the system tray area and
 * creates the popup menu associated with that icon. */
MainWindow::MainWindow()
: VidaliaWindow("MainWindow")
{
  VidaliaSettings settings;

  ui.setupUi(this);

  /* Create all the dialogs of which we only want one instance */
  _messageLog = new MessageLog();
  _bandwidthGraph = new BandwidthGraph();
  _netViewer = new NetViewer();
  
  /* Create the actions that will go in the tray menu */
  createActions();
  /* Creates a tray icon with a context menu and adds it to the system's
   * notification area. */
  createTrayIcon();
  /* Start with Tor initially stopped */
  updateTorStatus(Stopped);
  
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
    /* Tor may be already running, but we still need to connect to it. So,
     * update our status now. */ 
    updateTorStatus(Starting);
    /* Tor was already running */
    started();
  } else if (settings.runTorAtStart()) {
    /* If we're supposed to start Tor when Vidalia starts, then do it now */
    start();
  }
  
  if (isTrayIconSupported()) {
    /* Make the tray icon visible */
    _trayIcon.show();
    /* Check if we are supposed to show our main window on startup */
    ui.chkShowOnStartup->setChecked(settings.showMainWindowAtStart());
    if (ui.chkShowOnStartup->isChecked())
      show();
  } else {
    /* Don't let people hide the main window, since that's all they have. */
    ui.chkShowOnStartup->hide();
    ui.btnHide->hide();
    setMinimumHeight(height()-ui.btnHide->height());
    setMaximumHeight(height()-ui.btnHide->height());
    show();
  }
}

/** Destructor. */
MainWindow::~MainWindow()
{
  _trayIcon.hide();
  delete _messageLog;
  delete _netViewer;
  delete _bandwidthGraph;
}

/** Returns true if we're running on a platform with tray icon support. */
bool
MainWindow::isTrayIconSupported()
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
  /* We always have a tray on Win32 or a dock on OS X */
  return true;
#elif defined(USE_QSYSTEMTRAYICON)
  /* Ask Qt if there is a tray available */
  return QSystemTrayIcon::isSystemTrayAvailable();  
#else
  /* XXX: This is too optimistic, but we need to make our own tray icon
   * implementation smart enough to detect a system tray on X11. */
  return true;
#endif
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
  _startStopAct = new QAction(QIcon(IMG_START_TOR_16), tr("Start Tor"), this);
  connect(_startStopAct, SIGNAL(triggered()), this, SLOT(toggleTor()));

  _exitAct = new QAction(QIcon(IMG_EXIT), tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _bandwidthAct = new QAction(QIcon(IMG_BWGRAPH), tr("Bandwidth Graph"), this);
  connect(_bandwidthAct, SIGNAL(triggered()), 
          _bandwidthGraph, SLOT(showWindow()));
  connect(ui.lblBandwidthGraph, SIGNAL(clicked()),
          _bandwidthGraph, SLOT(showWindow()));

  _messageAct = new QAction(QIcon(IMG_MESSAGELOG), tr("Message Log"), this);
  connect(_messageAct, SIGNAL(triggered()),
          _messageLog, SLOT(showWindow()));
  connect(ui.lblMessageLog, SIGNAL(clicked()),
          _messageLog, SLOT(showWindow()));

  _networkAct = new QAction(QIcon(IMG_NETWORK), tr("Network Map"), this);
  connect(_networkAct, SIGNAL(triggered()), 
          _netViewer, SLOT(showWindow()));
  connect(ui.lblViewNetwork, SIGNAL(clicked()),
          _netViewer, SLOT(showWindow()));

  _controlPanelAct = new QAction(QIcon(IMG_CONTROL_PANEL), 
                                 tr("Control Panel"), this);
  connect(_controlPanelAct, SIGNAL(triggered()), this, SLOT(show()));

  _configAct = new QAction(QIcon(IMG_CONFIG), tr("Settings"), this);
  connect(_configAct, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
  
  _aboutAct = new QAction(QIcon(IMG_ABOUT), tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

  _helpAct = new QAction(QIcon(IMG_HELP), tr("Help"), this);
  connect(_helpAct, SIGNAL(triggered()), vApp, SLOT(help()));
  connect(ui.lblHelpBrowser, SIGNAL(clicked()), vApp, SLOT(help()));

  _newIdentityAct = new QAction(QIcon(IMG_IDENTITY), tr("New Identity"), this);
  _newIdentityAct->setEnabled(false);
  connect(_newIdentityAct, SIGNAL(triggered()), this, SLOT(newIdentity()));
}

/** Creates a tray icon with a context menu and adds it to the system
 * notification area. On Mac, we also set up an application menubar. */
void
MainWindow::createTrayIcon()
{
  /* Create the default menu bar (Mac) */
  createMenuBar();
  /* Create a tray menu and add it to the tray icon */
  _trayIcon.setContextMenu(createTrayMenu());

#if defined(USE_QSYSTEMTRAYICON)
  connect(&_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
#else
  connect(&_trayIcon, SIGNAL(doubleClicked()), this, SLOT(show()));
#endif
}

/** Creates a QMenu object that contains QActions which compose the system 
 * tray menu. */
QMenu* 
MainWindow::createTrayMenu()
{
  QMenu *menu = new QMenu(this);
  menu->addAction(_startStopAct);
  menu->addSeparator();
  menu->addAction(_bandwidthAct);
  menu->addAction(_messageAct);
  menu->addAction(_networkAct);
  menu->addAction(_newIdentityAct);
  
#if !defined(Q_WS_MAC)
  /* These aren't added to the dock menu on Mac, since they are in the
   * standard Mac locations in the menu bar. */
  menu->addSeparator();
  menu->addAction(_controlPanelAct);
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
  _startStopAct->setShortcut(tr("Ctrl+T"));
  _bandwidthAct->setShortcut(tr("Ctrl+B"));
  _messageAct->setShortcut(tr("Ctrl+L"));
  _networkAct->setShortcut(tr("Ctrl+N"));
  _helpAct->setShortcut(tr("Ctrl+?"));
  _newIdentityAct->setShortcut(tr("Ctrl+I"));
  _controlPanelAct->setShortcut(tr("Ctrl+P"));

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
  torMenu->addAction(_startStopAct);
  torMenu->addSeparator();
  torMenu->addAction(_newIdentityAct);

  QMenu *viewMenu = menuBar->addMenu(tr("View"));
  viewMenu->addAction(_controlPanelAct);
  viewMenu->addSeparator();
  viewMenu->addAction(_bandwidthAct);
  viewMenu->addAction(_messageAct);
  viewMenu->addAction(_networkAct);
  viewMenu->addAction(_configAct);
  
  QMenu *helpMenu = menuBar->addMenu(tr("Help"));
  _helpAct->setText(tr("Vidalia Help"));
  helpMenu->addAction(_helpAct);
  helpMenu->addAction(_aboutAct);
  
  setMenuBar(menuBar);
#endif
}

/** Updates the UI to reflect Tor's current <b>status</b>. */
void
MainWindow::updateTorStatus(TorStatus status)
{
  QString statusText, actionText;
  QString trayIconFile, statusIconFile;
  
  if (status == Stopped) {
      statusText = tr("Tor is not running");
      actionText = tr("Start Tor");
      trayIconFile = IMG_TOR_STOPPED;
      statusIconFile = IMG_TOR_STOPPED_48;
      _startStopAct->setEnabled(true);
      _startStopAct->setText(actionText);
      _startStopAct->setIcon(QIcon(IMG_START_TOR_16));
      ui.lblStartStopTor->setEnabled(true);
      ui.lblStartStopTor->setText(actionText);
      ui.lblStartStopTor->setPixmap(QPixmap(IMG_START_TOR_48));
      ui.lblStartStopTor->setStatusTip(actionText);
  } else if (status == Stopping) {
      if (_delayedShutdownStarted) {
        statusText = tr("Your Tor server is shutting down.\r\n" 
                        "Click 'Stop Tor' again to force Tor to stop now.");
      } else {
        statusText = tr("Tor is shutting down");
      }
      trayIconFile = IMG_TOR_STOPPING;
      statusIconFile = IMG_TOR_STOPPING_48;
      ui.lblNewIdentity->setEnabled(false);
      ui.lblStartStopTor->setStatusTip(tr("Stop Tor Now"));
  } else if (status == Running) {
      statusText = tr("Tor is running");
      actionText = tr("Stop Tor");
      trayIconFile = IMG_TOR_RUNNING;
      statusIconFile = IMG_TOR_RUNNING_48;
      _startStopAct->setEnabled(true);
      _startStopAct->setText(actionText);
      _startStopAct->setIcon(QIcon(IMG_STOP_TOR_16));
      ui.lblStartStopTor->setEnabled(true);
      ui.lblStartStopTor->setText(actionText);
      ui.lblStartStopTor->setPixmap(QPixmap(IMG_STOP_TOR_48));
      ui.lblStartStopTor->setStatusTip(actionText);
      _newIdentityAct->setEnabled(true);
      ui.lblNewIdentity->setEnabled(true);
  } else { /* status == Starting */
      statusText = tr("Tor is starting up");
      trayIconFile = IMG_TOR_STARTING;
      statusIconFile = IMG_TOR_STARTING_48;
      _startStopAct->setEnabled(false);
      ui.lblStartStopTor->setEnabled(false);
      ui.lblStartStopTor->setStatusTip(statusText);
      ui.lblStartStopTor->setAnimation(QPixmap(ANIM_PROCESS_WORKING));
  }

  /* Update the tray icon */
#if defined(USE_QSYSTEMTRAYICON)
  _trayIcon.setIcon(QIcon(trayIconFile));
#else
  _trayIcon.setIcon(trayIconFile);
#endif
  _trayIcon.setToolTip(statusText);
  
  /* Update the status banner */
  ui.lblTorStatus->setText(statusText);
  ui.lblTorStatusImg->setPixmap(QPixmap(statusIconFile));
}

#if defined(USE_QSYSTEMTRAYICON)
/** Displays the main window if <b>reason</b> is DoubleClick. */
void
MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::DoubleClick)
    show();
}
#endif

/** Called when the "show on startup" checkbox is toggled. */
void
MainWindow::toggleShowOnStartup(bool checked)
{
  VidaliaSettings settings;
  settings.setShowMainWindowAtStart(checked);
}

/** Starts Tor if it is not currently running, or stops Tor if it is already
 * running. */
void
MainWindow::toggleTor()
{
  if (_torControl->isRunning())
    stop();
  else
    start();
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
  updateTorStatus(Starting);
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
 
  updateTorStatus(Stopped);

  /* Display an error message and see if the user wants some help */
  int response = VMessageBox::warning(this, tr("Error Starting Tor"),
                   tr("Vidalia was unable to start Tor. Check your settings "
                        "to ensure the correct name and location of your Tor "
                        "executable is specified."),
                   VMessageBox::Ok, VMessageBox::ShowSettings, VMessageBox::Help);

  if (response == VMessageBox::ShowSettings) {
    /* Show the settings dialog so the user can make sure they're pointing to
     * the correct Tor. */
     showConfigDialog();
  } else if (response == VMessageBox::Help) {
    /* Show troubleshooting information about starting Tor */
    Vidalia::help("troubleshooting.start");
  }
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
    if (response == VMessageBox::Help)
      Vidalia::help("troubleshooting.connect");
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
    rc = _torControl->stop(&errmsg);
  }
  
  if (rc) {
    /* Indicate that Tor is about to shut down */
    updateTorStatus(Stopping);
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
  updateTorStatus(Stopped);

  /* If we didn't intentionally close Tor, then check to see if it crashed or
   * if it closed itself and returned an error code. */
  if (!_isIntentionalExit) {
    /* A quick overview of Tor's code tells me that if it catches a SIGTERM or
     * SIGINT, Tor will exit(0). We might need to change this warning message
     * if this turns out to not be the case. */
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
      int ret = VMessageBox::warning(this, tr("Tor Exited"),
                  tr("Vidalia detected that Tor exited unexpectedly.\n\n"
                     "Please check the message log for indicators "
                     "about what happened to Tor before it exited."),
                  VMessageBox::Ok, VMessageBox::ShowLog, VMessageBox::Help);
      if (ret == VMessageBox::ShowLog)
        _messageLog->showWindow();  
      else if (ret == VMessageBox::Help)
        Vidalia::help("troubleshooting.torexited");
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
  updateTorStatus(Running);
  
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
        showConfigDialog(ConfigDialog::Server);
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
  ui.lblNewIdentity->setEnabled(false);
}

/** Creates and displays Vidalia's About dialog. */
void
MainWindow::showAboutDialog()
{
  static AboutDialog *aboutDialog = 0;
  if (!aboutDialog)
    aboutDialog = new AboutDialog(this);
  aboutDialog->showWindow();
}

/** Creates and displays the Configuration dialog with the current page set to
 * <b>page</b>. */
void
MainWindow::showConfigDialog(ConfigDialog::Page page)
{
  static ConfigDialog *configDialog = 0;
  if (!configDialog)
    configDialog = new ConfigDialog(this);
  configDialog->showWindow(page);
}

/** Displays the Configuration dialog, set to the Server page. */
void
MainWindow::showServerConfigDialog()
{
  showConfigDialog(ConfigDialog::Server);
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

    /* Disable the New Identity button for MIN_NEWIDENTITY_INTERVAL */
    _newIdentityAct->setEnabled(false);
    ui.lblNewIdentity->setEnabled(false);
    QTimer::singleShot(MIN_NEWIDENTITY_INTERVAL, 
                       this, SLOT(enableNewIdentity()));
    
#if defined(USE_QSYSTEMTRAYICON)
    if (QSystemTrayIcon::supportsMessages())
      _trayIcon.showMessage(title, message, QSystemTrayIcon::Information);
    else
      VMessageBox::information(this, title, message, VMessageBox::Ok);
#else
    VMessageBox::information(this, title, message, VMessageBox::Ok);
#endif
  } else {
    /* NEWNYM signal failed */
    VMessageBox::warning(this, 
      tr("Failed to Create New Identity"), errmsg, VMessageBox::Ok);
  }
}

/** Re-enables the 'New Identity' button after a delay from the previous time
 * 'New Identity' was used. */
void
MainWindow::enableNewIdentity()
{
  if (_torControl->isConnected()) {
    _newIdentityAct->setEnabled(true);
    ui.lblNewIdentity->setEnabled(true);
  }
}

