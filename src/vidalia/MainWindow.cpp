/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

/*
** \file MainWindow.cpp
** \brief Main (hidden) window. Creates tray menu and child windows
**
** Implements the main window. The main window is a hidden window that serves
** as the parent of the tray icon and popup menu, as well as other application
** dialogs.
*/

#include "MainWindow.h"
#include "Vidalia.h"
#include "ControlPasswordInputDialog.h"
#include "VMessageBox.h"
#include "TorSettings.h"
#include "ServerSettings.h"
#include "AboutDialog.h"
#include "HelpBrowser.h"
#include "VAttachButton.h"
#ifdef USE_AUTOUPDATE
#include "UpdatesAvailableDialog.h"
#endif

#include "ProtocolInfo.h"

#include "net.h"
#include "file.h"
#include "html.h"
#include "stringutil.h"
#include "procutil.h"

#include "PluginWrapper.h"
#include "DebugDialog.h"

#include <QtGui>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

#define IMG_BWGRAPH        ":/images/16x16/utilities-system-monitor.png"
#define IMG_CONTROL_PANEL  ":/images/16x16/system-run.png"
#define IMG_MESSAGELOG     ":/images/16x16/format-justify-fill.png"
#define IMG_CONFIG         ":/images/16x16/preferences-system.png"
#define IMG_IDENTITY       ":/images/16x16/view-media-artist.png"
#define IMG_HELP           ":/images/16x16/system-help.png"
#define IMG_ABOUT          ":/images/16x16/help-about.png"
#define IMG_EXIT           ":/images/16x16/application-exit.png"
#define IMG_NETWORK        ":/images/16x16/applications-internet.png"

#define IMG_START_TOR_16     ":/images/16x16/media-playback-start.png"
#define IMG_STOP_TOR_16      ":/images/16x16/media-playback-stop.png"
#define IMG_START_TOR_48     ":/images/48x48/media-playback-start.png"
#define IMG_STOP_TOR_48      ":/images/48x48/media-playback-stop.png"
#define IMG_TOR_STOPPED_48   ":/images/48x48/tor-off.png"
#define IMG_TOR_RUNNING_48   ":/images/48x48/tor-on.png"
#define IMG_TOR_STARTING_48  ":/images/48x48/tor-starting.png"
#define IMG_TOR_STOPPING_48  ":/images/48x48/tor-stopping.png"

/* Decide which of our four sets of tray icons to use. */
#if defined(Q_WS_WIN)
/* QSystemTrayIcon on Windows wants 16x16 .png files */
#define IMG_TOR_STOPPED  ":/images/16x16/tor-off.png"
#define IMG_TOR_RUNNING  ":/images/16x16/tor-on.png"
#define IMG_TOR_STARTING ":/images/16x16/tor-starting.png"
#define IMG_TOR_STOPPING ":/images/16x16/tor-stopping.png"
#elif defined(Q_WS_MAC)
/* On Mac, the dock icons look best at 128x128, otherwise they get blurry
 * if resized from a smaller image */
#define IMG_TOR_STOPPED    ":/images/128x128/tor-off.png"
#define IMG_TOR_RUNNING    ":/images/128x128/tor-on.png"
#define IMG_TOR_STARTING   ":/images/128x128/tor-starting.png"
#define IMG_TOR_STOPPING   ":/images/128x128/tor-stopping.png"
void qt_mac_set_dock_menu(QMenu *menu);
#else
/* On X11, we just use always the 22x22 .png files */
#define IMG_TOR_STOPPED    ":/images/22x22/tor-off.png"
#define IMG_TOR_RUNNING    ":/images/22x22/tor-on.png"
#define IMG_TOR_STARTING   ":/images/22x22/tor-starting.png"
#define IMG_TOR_STOPPING   ":/images/22x22/tor-stopping.png"
#endif

/** Only allow 'New Identity' to be clicked once every 10 seconds. */
#define MIN_NEWIDENTITY_INTERVAL   (10*1000)

/* Startup progress milestones */
#define STARTUP_PROGRESS_STARTING          0
#define STARTUP_PROGRESS_CONNECTING       10
#define STARTUP_PROGRESS_AUTHENTICATING   20
#define STARTUP_PROGRESS_BOOTSTRAPPING    30
#define STARTUP_PROGRESS_CIRCUITBUILD     75
#define STARTUP_PROGRESS_MAXIMUM          (STARTUP_PROGRESS_BOOTSTRAPPING+100)

/** Default constructor. It installs an icon in the system tray area and
 * creates the popup menu associated with that icon. */
MainWindow::MainWindow()
: VidaliaWindow("MainWindow")
{
  _startedWithPrevious = false;

  /* Create a new TorControl object, used to communicate with Tor */
  _torControl = Vidalia::torControl(); 

  _engine = new PluginEngine();

  _dummy = new QAction(tr("No dettached tabs"), this);

  createGUI();
  createConnections();

  /* Start with Tor initially stopped */
  _status = Unset;
  _isVidaliaRunningTor = false;
  updateTorStatus(Stopped);

#if defined(Q_WS_MAC)
  VidaliaSettings settings;
  /* Display OSX dock icon if icon preference is not set to "Tray Only" */
  if (settings.getIconPref() != VidaliaSettings::Tray) {
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
  }
  /* Vidalia launched in background (LSUIElement=true). Bring to foreground. */
  VidaliaWindow::setVisible(true);
#endif
}

/** Destructor */
MainWindow::~MainWindow()
{
  delete _engine;
}

/** Calls the different methods that will handle the GUI "creation".
 * It's called once at the MainWindow creation. */
void
MainWindow::createGUI()
{
  ui.setupUi(this);
  createActions();
  createMenuBar();
  createToolBar();
  createTrayIcon();

  // We need to create this tab at the beggining
  // and we must specify the statusBar
  _messageLog = new MessageLog(this->statusBar());
  _graph = new BandwidthGraph(this->statusBar());

  addTab(&_statusTab);
  ui.tabWidget->pinTab(0);
}

/** Creates the actions used in toolbars and menu */
void
MainWindow::createActions()
{
  _actionShowControlPanel = new QAction(QIcon(IMG_CONTROL_PANEL), tr("Control Panel"), this);
  _actionRestartTor = new QAction(tr("Restart"), this);
  _actionReloadConfig = new QAction(tr("Reload Tor's config"), this);
  _actionNewIdentity = new QAction(QIcon(IMG_IDENTITY), tr("New Identity"), this);
  _actionStatus = new QAction(QIcon(IMG_CONTROL_PANEL), tr("Status"), this);
  _actionNetworkMap = new QAction(QIcon(IMG_NETWORK), tr("Network Map"), this);
  _actionMessageLog= new QAction(QIcon(IMG_MESSAGELOG), tr("Message Log"), this);
  _actionBandwidthGraph = new QAction(QIcon(IMG_BWGRAPH), tr("Bandwidth Graph"), this);
  _actionConfigure = new QAction(QIcon(IMG_CONFIG), tr("Settings"), this);
  _actionVidaliaHelp = new QAction(QIcon(IMG_HELP), tr("Help"), this);
  _actionAbout = new QAction(QIcon(IMG_ABOUT), tr("About"), this);
  _actionStartStopTor = new QAction(QIcon(IMG_START_TOR_16), tr("Start Tor"), this);
  _actionExit = new QAction(QIcon(IMG_EXIT), tr("Exit"), this);
  _actionDebugDialog = new QAction(tr("Debug output"), this);
}

/** Creates the menu bar */
void
MainWindow::createMenuBar()
{
  _reattachMenu.setTitle(tr("Reattach tabs"));

  QMenuBar *menu = menuBar();
  menu->clear();

  QMenu *torMenu = menu->addMenu(tr("Tor"));
  torMenu->addAction(_actionStartStopTor);
  torMenu->addAction(_actionRestartTor);
#if !defined(Q_WS_WIN)
  torMenu->addAction(_actionReloadConfig);
#endif
  torMenu->addSeparator();
  torMenu->addAction(_actionExit);

  QMenu *actionsMenu = menu->addMenu(tr("Actions"));
  actionsMenu->addAction(_actionNewIdentity);

  QMenu *viewMenu = menu->addMenu(tr("View"));
  viewMenu->addAction(_actionStatus);
  viewMenu->addAction(_actionNetworkMap);
  viewMenu->addAction(_actionMessageLog);
  viewMenu->addAction(_actionBandwidthGraph);
  viewMenu->addSeparator();
  viewMenu->addAction(_actionConfigure);

  QMenu *pluginsMenu = menu->addMenu(tr("Plugins"));
  foreach(QAction *action, _engine->getAllActions()) {
    pluginsMenu->addAction(action);
  }
  pluginsMenu->addSeparator();
  pluginsMenu->addAction(_actionDebugDialog);

  menu->addMenu(&_reattachMenu);
  _reattachMenu.addAction(_dummy);
  _dummy->setEnabled(false);

  QMenu *helpMenu = menu->addMenu(tr("Help"));
  helpMenu->addAction(_actionVidaliaHelp);
  helpMenu->addSeparator();
  helpMenu->addAction(_actionAbout);
}

/** Creates the main toolbar */
void
MainWindow::createToolBar()
{
  QToolBar *tool = ui.toolBar;

  tool->addAction(_actionStartStopTor);
  tool->addAction(_actionNewIdentity);
  tool->addAction(_actionConfigure);

  tool->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

/** Creates a QMenu object that contains QActions which compose the system 
 * tray menu. */
QMenu* 
MainWindow::createTrayMenu()
{
  QMenu *menu = new QMenu(this);
  menu->addAction(_actionStartStopTor);
  menu->addSeparator();
  menu->addAction(_actionBandwidthGraph);
  menu->addAction(_actionMessageLog);
  menu->addAction(_actionNetworkMap);
  menu->addAction(_actionNewIdentity);
  menu->addSeparator();
  menu->addAction(_actionShowControlPanel);

  menu->addMenu(&_reattachMenu);
  
#if !defined(Q_WS_MAC)
  /* These aren't added to the dock menu on Mac, since they are in the
   * standard Mac locations in the menu bar. */
  menu->addAction(_actionConfigure);
  menu->addAction(_actionVidaliaHelp);
  menu->addAction(_actionAbout);
  menu->addSeparator();
  menu->addAction(_actionExit);
#endif
  return menu;
}

/** Called when tor detects a problem with the system clock */
void
MainWindow::clockSkewed(int skew, const QString &source)
{
  if (source.startsWith("OR:", Qt::CaseInsensitive)) {
    // Tor versions 0.2.1.19 and earlier, and 0.2.2.1 and earlier, throw
    // this message a little too liberally in this case.
    quint32 torVersion = Vidalia::torControl()->getTorVersion();
    if (torVersion <= 0x00020113)
      return;
    QString str = Vidalia::torControl()->getTorVersionString();
    if (str.startsWith("0.2.2.") && torVersion <= 0x00020201)
      return;
  }

  _trayIcon.showMessage(tr("Your clock is wrong"),
                        tr("Your computer's clock is wrong, tor may not work as "
                           "expected. Please check the Message Log for more information."));
}


/** Creates a tray icon with a context menu and adds it to the system
 * notification area. On Mac, we also set up an application menubar. */
void
MainWindow::createTrayIcon()
{
  QMenu *menu = createTrayMenu();

  /* Add the menu it to the tray icon */
  _trayIcon.setContextMenu(menu);

  connect(&_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

#if defined(Q_WS_MAC)
  qt_mac_set_dock_menu(menu);
#endif

  VidaliaSettings settings;

  _statusTab.checkShowOnStartup(settings.showMainWindowAtStart());
  if (_statusTab.isCheckedShowOnStartup())
    show(); 
  _trayIcon.show();
}

void
MainWindow::setVisible(bool visible)
{
  if (visible) {
    /* In Gnome, will hide buttons if Vidalia is run on startup. */
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
      /* Don't let people hide the main window, since that's all they have. */
      _statusTab.hideCheckShowOnStartup();
      /* Causes window to not appear in Enlightenment. */
      //setMinimumHeight(height()-ui.btnHide->height());
      //setMaximumHeight(height()-ui.btnHide->height());
    }
  }
  VidaliaWindow::setVisible(visible);
}

/** Respond to a double-click on the tray icon by opening the Control Panel
 * window. */
void
MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::DoubleClick)
    setVisible(!isVisible());
}

void
MainWindow::retranslateUi()
{
  updateTorStatus(_status);

  _actionShowControlPanel->setText(tr("Control Panel"));
  _actionRestartTor->setText(tr("Restart"));
  _actionReloadConfig->setText(tr("Reload Tor's config"));
  _actionConfigure->setText(tr("Settings"));

  if (_status == Stopped) {
    _actionStartStopTor->setText(tr("Start Tor"));
  } else if (_status == Starting) {
    _actionStartStopTor->setText(tr("Starting Tor"));
  } else {
    _actionStartStopTor->setText(tr("Stop Tor"));
  }

  _actionBandwidthGraph->setText(tr("Bandwidth Graph"));
  _actionMessageLog->setText(tr("Message Log"));
  _actionNetworkMap->setText(tr("Network Map"));
  _actionStatus->setText(tr("Status"));
  _actionVidaliaHelp->setText(tr("Help"));
  _actionNewIdentity->setText(tr("New Identity"));

#if !defined(Q_WS_MAC)
  _actionAbout->setText(tr("About"));
  _actionConfigure->setText(tr("Settings"));
  _actionExit->setText(tr("Exit"));
#endif

  createMenuBar();
  ui.retranslateUi(this);
}

void
MainWindow::createConnections()
{
  connect(_actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(_actionStartStopTor, SIGNAL(triggered()), this, SLOT(start()));
  connect(_actionRestartTor, SIGNAL(triggered()), this, SLOT(restart()));
  connect(_actionReloadConfig, SIGNAL(triggered()), this, SLOT(sighup()));
  connect(_actionShowControlPanel, SIGNAL(triggered()), this, SLOT(show()));
  connect(_actionNewIdentity, SIGNAL(triggered()), this, SLOT(newIdentity()));

  connect(_actionMessageLog, SIGNAL(triggered()), this, SLOT(showMessageLogTab()));
  connect(_actionConfigure, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
  connect(_actionBandwidthGraph, SIGNAL(triggered()), this, SLOT(showBandwidthTab()));
  connect(_actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
  connect(_actionVidaliaHelp, SIGNAL(triggered()), this, SLOT(showHelpDialog()));
  connect(_actionStatus, SIGNAL(triggered()), this, SLOT(showStatusTab()));
  connect(_actionNetworkMap, SIGNAL(triggered()), this, SLOT(showNetViewerTab()));

  connect(_actionDebugDialog, SIGNAL(triggered()), this, SLOT(showDebugDialog()));

  /* Catch signals when the application is running or shutting down */
  connect(vApp, SIGNAL(running()), this, SLOT(running()));
  connect(vApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

  connect(_torControl, SIGNAL(started()), this, SLOT(started()));
  connect(_torControl, SIGNAL(startFailed(QString)),
          this, SLOT(startFailed(QString)));
  connect(_torControl, SIGNAL(stopped(int, QProcess::ExitStatus)),
          this, SLOT(stopped(int, QProcess::ExitStatus)));
  connect(_torControl, SIGNAL(connected()), this, SLOT(connected()));
  connect(_torControl, SIGNAL(disconnected()), this, SLOT(disconnected()));
  connect(_torControl, SIGNAL(connectFailed(QString)), 
          this, SLOT(connectFailed(QString)));
  connect(_torControl, SIGNAL(authenticated()), this, SLOT(authenticated()));
  connect(_torControl, SIGNAL(authenticationFailed(QString)),
          this, SLOT(authenticationFailed(QString)));
  connect(_torControl, SIGNAL(clockSkewed(int, QString)),
          this, SLOT(clockSkewed(int, QString)));

  _torControl->setEvent(TorEvents::GeneralStatus);
  connect(_torControl, SIGNAL(dangerousTorVersion(tc::TorVersionStatus,
                                                  QString, QStringList)),
          this, SLOT(dangerousTorVersion(tc::TorVersionStatus,
                                         QString, QStringList)));

  _torControl->setEvent(TorEvents::ClientStatus);
  connect(_torControl, SIGNAL(bootstrapStatusChanged(BootstrapStatus)),
          this, SLOT(bootstrapStatusChanged(BootstrapStatus)));
  connect(_torControl, SIGNAL(circuitEstablished()),
          this, SLOT(circuitEstablished()));
  connect(_torControl, SIGNAL(dangerousPort(quint16, bool)),
          this, SLOT(warnDangerousPort(quint16, bool)));

  connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)),
          this, SLOT(handleCloseTab(int)));

#if defined(USE_AUTOUPDATE)
  /* Create a timer used to remind us to check for software updates */
  connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(checkForUpdates()));

  /* Also check for updates in the foreground when the user clicks the
   * "Check Now" button in the config dialog. */
  connect(_configDialog, SIGNAL(checkForUpdates()),
          this, SLOT(checkForUpdatesWithUi()));

  /* The rest of these slots are called as the update process executes. */
  connect(&_updateProcess, SIGNAL(downloadProgress(QString,int,int)),
          &_updateProgressDialog, SLOT(setDownloadProgress(QString,int,int)));
  connect(&_updateProcess, SIGNAL(updatesAvailable(UpdateProcess::BundleInfo,PackageList)),
          this, SLOT(updatesAvailable(UpdateProcess::BundleInfo,PackageList)));
  connect(&_updateProcess, SIGNAL(updatesInstalled(int)),
          this, SLOT(updatesInstalled(int)));
  connect(&_updateProcess, SIGNAL(installUpdatesFailed(QString)),
          this, SLOT(installUpdatesFailed(QString)));
  connect(&_updateProgressDialog, SIGNAL(cancelUpdate()),
          &_updateProcess, SLOT(cancel()));
#endif

#if defined(USE_MINIUPNPC)
  /* Catch UPnP-related signals */
  connect(UPNPControl::instance(), SIGNAL(error(UPNPControl::UPNPError)),
          this, SLOT(upnpError(UPNPControl::UPNPError)));
#endif

  connect(_engine, SIGNAL(pluginTab(VidaliaTab *)), 
          this, SLOT(addTab(VidaliaTab *)));
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
      connect(_torControl, SIGNAL(stopped()), vApp, SLOT(quit()));
      if (!stop())
        QObject::disconnect(_torControl, SIGNAL(stopped()), vApp, SLOT(quit()));
      return;
    }
  }
  vApp->quit();
}

/** Called when the application has started and the main event loop is
 * running. */
void
MainWindow::running()
{
  VidaliaSettings settings;

  if (vApp->readPasswordFromStdin()) {
    QTextStream in(stdin);
    in >> _controlPassword;
    _useSavedPassword = false;
  } else {
    /* Initialize _useSavedPassword to true. If Tor is already running when
     * Vidalia starts, then there is no point in generating a random password.
     * If Tor is not already running, then this will be set according to the
     * current configuration in the start() method.
     */
    _useSavedPassword = true;
  }

  if (settings.runTorAtStart()) {
    /* If we're supposed to start Tor when Vidalia starts, then do it now */
    start();
  }

#if defined(USE_AUTOUPDATE)
  if (settings.isAutoUpdateEnabled()) {
    QDateTime lastCheckedAt = settings.lastCheckedForUpdates();
    if (UpdateProcess::shouldCheckForUpdates(lastCheckedAt)) {
      if (settings.runTorAtStart() && ! _torControl->isCircuitEstablished()) {
        /* We started Tor but it hasn't bootstrapped yet, so give it a bit
         * before we decide to check for updates. If Tor manages to build a
         * circuit before this timer times out, we will stop the timer and
         * launch a check for updates immediately. (see circuitEstablished()).
         */
        _updateTimer.start(5*60*1000);
      } else {
        /* Initiate a background check for updates now */
        checkForUpdates();
      }
    } else {
      /* Schedule the next time to check for updates */
      QDateTime nextCheckAt = UpdateProcess::nextCheckForUpdates(lastCheckedAt);
      QDateTime now = QDateTime::currentDateTime().toUTC();

      vInfo("Last checked for software updates at %1. Will check again at %2.")
        .arg(lastCheckedAt.toLocalTime().toString("dd-MM-yyyy hh:mm:ss"))
        .arg(nextCheckAt.toLocalTime().toString("dd-MM-yyyy hh:mm:ss"));
      _updateTimer.start((nextCheckAt.toTime_t() - now.toTime_t()) * 1000);
    }
  }
#endif
}

/** Terminate the Tor process if it is being run under Vidalia, disconnect all
 * TorControl signals, and exit Vidalia. */
void
MainWindow::aboutToQuit()
{
  vNotice("Cleaning up before exiting.");

  if (_torControl->isVidaliaRunningTor()) {
    /* Kill our Tor process now */ 
    _torControl->stop();
  }

  /* Disable port forwarding */
  ServerSettings settings(_torControl);
  settings.cleanupPortForwarding();

  /* Disconnect all of the TorControl object's signals */
  QObject::disconnect(_torControl, 0, 0, 0);
}


/** Attempts to start Tor. If Tor fails to start, then startFailed() will be
 * called with an error message containing the reason. */
void 
MainWindow::start()
{
  TorSettings settings;
  QStringList args;

  updateTorStatus(Starting);

  // Disable autoconfiguration if there are missing config data
  if(settings.autoControlPort()) {
    if(settings.getDataDirectory().isEmpty()) {
      vWarn("Disabling ControlPort autoconfiguration. DataDirectory is empty!");
      settings.setAutoControlPort(false);
    }
  }


  /* Check if Tor is already running separately */
  if(settings.getControlMethod() == ControlMethod::Port) {
    if(!settings.autoControlPort() && net_test_connect(settings.getControlAddress(),
                                                       settings.getControlPort())) {
      started();
      return;
    }
  } else {
    if (socket_test_connect(settings.getSocketPath())) {
      started();
      return;
    }
  }

  QString torrc = settings.getTorrc();

  if(settings.bootstrap()) {
    QString boottorrc = settings.bootstrapFrom();
    vNotice(tr("Bootstrapping torrc from %1 to %2")
        .arg(boottorrc).arg(torrc));
    if(QFileInfo(boottorrc).exists()) {
      if(QFile::copy(boottorrc, torrc)) {
        settings.setBootstrap(false);
      }
    }
  }

  /* Make sure the torrc we want to use really exists. */
  if (!torrc.isEmpty()) {
    if (!QFileInfo(torrc).exists())
      touch_file(torrc, true);
    args << "-f" << torrc;
  }

  /* Specify Tor's data directory, if different from the default */
  QString dataDirectory = settings.getDataDirectory();
  QString expDataDirectory = expand_filename(dataDirectory);
  if (!dataDirectory.isEmpty())
    args << "DataDirectory" << expDataDirectory;
  
  if(settings.getControlMethod() == ControlMethod::Port) {
    if(settings.autoControlPort()) {
      QString portconf = QString("%1/port.conf").arg(expDataDirectory);
      if(!QFile::remove(portconf))
        vWarn(QString("Unable to remove %s, may be it didn't existed.").arg(portconf));

      QString control_str = "auto";
      QString socks_str = "auto";

      if(!_previousControlPort.isEmpty()) {
        control_str = _previousControlPort;
        _startedWithPrevious = true;
      }
      if(!_previousSocksPort.isEmpty()) {
        socks_str = _previousSocksPort;
        _startedWithPrevious = true;
      }

      args << "ControlPort" << control_str;
      args << "SocksPort" << socks_str;
      args << "ControlPortWriteToFile" << QString("%1/port.conf").arg(expDataDirectory);
    } else {
      /* Add the intended control port value */
      quint16 controlPort = settings.getControlPort();
      if (controlPort)
        args << "ControlPort" << QString::number(controlPort);
    }
  } else {
    QString path = settings.getSocketPath();
    args << "ControlSocket" << path;
  }

  args << "__OwningControllerProcess" << QString::number(QCoreApplication::applicationPid());
  
  /* Add the control port authentication arguments */
  switch (settings.getAuthenticationMethod()) {
    case TorSettings::PasswordAuth:
      if (! vApp->readPasswordFromStdin()) {
        if (settings.useRandomPassword()) {
          _controlPassword = TorSettings::randomPassword();
          _useSavedPassword = false;
        } else {
          _controlPassword = settings.getControlPassword();
          _useSavedPassword = true;
        }
      }
      args << "HashedControlPassword"
           << TorSettings::hashPassword(_controlPassword);

      break;
    case TorSettings::CookieAuth:
      args << "CookieAuthentication"  << "1";
      break;
    default:
      args << "CookieAuthentication"  << "0";
  }

  /* This doesn't get set to false until Tor is actually up and running, so we
   * don't yell at users twice if their Tor doesn't even start, due to the fact
   * that QProcess::stopped() is emitted even if the process didn't even
   * start. */
  _isIntentionalExit = true;
  /* Kick off the Tor process */
  _torControl->start(settings.getExecutable(), args);
}

/** Slot: Called when the Tor process is started. It will connect the control
 * socket and set the icons and tooltips accordingly. */
void 
MainWindow::started()
{
  TorSettings settings;

  updateTorStatus(Started);

  /* Now that Tor is running, we want to know if it dies when we didn't want
   * it to. */
  _isIntentionalExit = false;
  /* We haven't started a delayed shutdown yet. */
  _delayedShutdownStarted = false;
  /* Remember whether we started Tor or not */
  _isVidaliaRunningTor = _torControl->isVidaliaRunningTor();

  if(settings.autoControlPort()) {
    QString dataDirectory = settings.getDataDirectory();
    QFile file(QString("%1/port.conf").arg(expand_filename(dataDirectory)));
    int tries = 0, maxtries = 5;
    while((!file.open(QIODevice::ReadOnly | QIODevice::Text)) and
          (tries++ < maxtries)) {
      vWarn(QString("This is try number: %1.").arg(tries));
#if defined(Q_WS_WIN)
      Sleep(1000);
#else
      sleep(1);
#endif
    }

    if(tries >= maxtries) {
      vWarn("Couldn't read port.conf file");
      if(_torControl->isRunning()) {
        connectFailed(tr("Vidalia can't find out how to talk to Tor because it can't access this file: %1\n\nHere's the last error message:\n %2")
                      .arg(file.fileName())
                      .arg(file.errorString()));
      } else {
        vWarn("Tor isn't running!");
        connectFailed(tr("It seems Tor has stopped running since Vidalia started it.\n\nSee the Advanced Message Log for more information."));
      }
      return;
    }

    QTextStream in(&file);
    if(!in.atEnd()) {
      QString line = in.readLine();
      QStringList parts = line.split("=");
      if(parts.size() != 2) return;
      if(parts[0].trimmed() != "PORT") return;

      QStringList addrPort = parts[1].split(":");
      if(addrPort.size() != 2) return;

      QHostAddress addr(addrPort.at(0));
      quint16 port = addrPort.at(1).toInt();
      _torControl->connect(addr, port);
    }

    file.close();
  } else {
    /* Try to connect to Tor's control port */
    if(settings.getControlMethod() == ControlMethod::Port)
      _torControl->connect(settings.getControlAddress(),
                          settings.getControlPort());
    else
      _torControl->connect(settings.getSocketPath());
  }
  setStartupProgress(STARTUP_PROGRESS_CONNECTING, tr("Connecting to Tor"));
}

/** Disconnects the control socket and stops the Tor process. */
bool
MainWindow::stop()
{
  ServerSettings server(_torControl);
  QString errmsg;
  TorStatus prevStatus;
  bool rc;
  VidaliaSettings settings;

  /* If we're running a server, give users the option of terminating
   * gracefully so clients have time to find new servers. */
  if (server.isServerEnabled() && !_delayedShutdownStarted) {
    /* Ask the user if they want to shutdown nicely. */
    int response = VMessageBox::Yes;
    if(!settings.rememberShutdown()) {
      response = VMessageBox::question(this, tr("Relaying is Enabled"),
                     tr("You are currently running a relay. "
                        "Terminating your relay will interrupt any "
                        "open connections from clients.\n\n"
                        "Would you like to shutdown gracefully and "
                        "give clients time to find a new relay?"),
                        VMessageBox::Yes|VMessageBox::Default, 
                        VMessageBox::No, 
                        VMessageBox::Cancel|VMessageBox::Escape,
                        "Remember this answer", &settings, SETTING_REMEMBER_SHUTDOWN);
    }
    if (response == VMessageBox::Yes)
      _delayedShutdownStarted = true;
    else if (response == VMessageBox::Cancel)
      return false;
  }
  
  prevStatus = updateTorStatus(Stopping);  
  if (_delayedShutdownStarted) {
    /* Start a delayed shutdown */
    rc = _torControl->signal(TorSignal::Shutdown, &errmsg);
  } else {
    /* We want Tor to stop now, regardless of whether we're a server. */
    _isIntentionalExit = true;
    rc = _torControl->stop(&errmsg);
  }
  
  if (!rc) {
    /* We couldn't tell Tor to stop, for some reason. */
    int response = VMessageBox::warning(this, tr("Error Shutting Down"),
                     p(tr("Vidalia was unable to stop the Tor software.")) 
                       + p(errmsg),
                     VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape, 
                     VMessageBox::Help);
      
    if (response == VMessageBox::Help) {
      /* Show some troubleshooting help */
      showHelpDialog("troubleshooting.stop");
    }
    /* Tor is still running since stopping failed */
    _isIntentionalExit = false;
    _delayedShutdownStarted = false;
    updateTorStatus(prevStatus);
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
      int ret = VMessageBox::warning(this, tr("Unexpected Error"),
                  tr("Vidalia detected that the Tor software exited "
                     "unexpectedly.\n\n"
                     "Please check the message log for recent "
                     "warning or error messages."),
                  VMessageBox::Ok|VMessageBox::Escape, 
                  VMessageBox::ShowLog|VMessageBox::Default,
                  VMessageBox::Help);
      if (ret == VMessageBox::ShowLog)
        showMessageLogTab();
      else if (ret == VMessageBox::Help)
        showHelpDialog("troubleshooting.torexited");
    }
  }
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

  if(_startedWithPrevious) {
    _startedWithPrevious = false;
    _previousControlPort = "";
    _previousSocksPort = "";
    vWarn("Retrying with new ports");
    start();
    return;
  }
 
  updateTorStatus(Stopped);

  /* Display an error message and see if the user wants some help */
  int response = VMessageBox::warning(this, tr("Error Starting Tor"),
                   tr("Vidalia was unable to start Tor. Check your settings "
                        "to ensure the correct name and location of your Tor "
                        "executable is specified."),
                   VMessageBox::ShowSettings|VMessageBox::Default,
                   VMessageBox::Cancel|VMessageBox::Escape,
                   VMessageBox::Help);

  if (response == VMessageBox::ShowSettings) {
    /* Show the settings dialog so the user can make sure they're pointing to
     * the correct Tor. */
     showConfigDialog();
  } else if (response == VMessageBox::Help) {
    /* Show troubleshooting information about starting Tor */
    showHelpDialog("troubleshooting.start");
  }
}

/** Called when the control socket has successfully connected to Tor. */
void
MainWindow::connected()
{
  authenticate();
  if(!_torControl->isVidaliaRunningTor()) {
    QString err;
    if(!_torControl->takeOwnership(&err))
      vWarn(err);
  }

  TorSettings settings;
  if(settings.autoControlPort()) {
    // We want to remember the ports if it's on auto
    QString control_str = "", socks_str = "";
    if(_torControl->getInfo("net/listeners/control", control_str)) {
      QStringList control_parts = control_str.split(":");
      if(control_parts.size() > 1)
        control_str = control_parts[1];
    }
    if(_torControl->getInfo("net/listeners/socks", socks_str)) {
      QStringList socks_parts = socks_str.split(":");
      if(socks_parts.size() > 1)
        socks_str = socks_parts[1];
    }
    
    _previousControlPort = control_str;
    _previousSocksPort = socks_str;
  } else {
    // Otherwise we want to clear the remembered ports
    _previousControlPort = "";
    _previousSocksPort = "";
  }
}

/** Called when the connection to the control socket fails. The reason will be
 * given in the errmsg parameter. */
void
MainWindow::connectFailed(QString errmsg)
{
  /* Ok, ok. It really isn't going to connect. I give up. */
  int response = VMessageBox::warning(this, 
                   tr("Connection Error"), p(errmsg),
                   VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape, 
                   VMessageBox::Retry, VMessageBox::Help);


  if (response == VMessageBox::Retry) {
    /* Let's give it another try. */
    TorSettings settings;
    _torControl->connect(settings.getControlAddress(),
                         settings.getControlPort());
  } else {
    /* Show the help browser (if requested) */
    if (response == VMessageBox::Help)
      showHelpDialog("troubleshooting.connect");
    /* Since Vidalia can't connect, we can't really do much, so stop Tor. */
    _torControl->stop();
  }
}

/** Called when Vidalia has successfully authenticated to Tor. */
void
MainWindow::authenticated()
{
  ServerSettings serverSettings(_torControl);
  QString errmsg;

  updateTorStatus(Authenticated);
  
  /* If Tor doesn't have bootstrapping events, then update the current
   * status string and bump the progress bar along a bit. */
  if (_torControl->getTorVersion() < 0x020101) {
    setStartupProgress(STARTUP_PROGRESS_CIRCUITBUILD,
                       tr("Connecting to the Tor network"));
  }
  
  /* Let people click on their beloved "New Identity" button */
  _actionNewIdentity->setEnabled(true);

  /* Register for any pertinent asynchronous events. */
  if (!_torControl->setEvents(&errmsg)) {
    VMessageBox::warning(this, tr("Error Registering for Events"),
      p(tr("Vidalia was unable to register for some events. "
           "Many of Vidalia's features may be unavailable."))
         + p(errmsg),
      VMessageBox::Ok);
  } else {
    /* Stop reading from Tor's stdout immediately, since we successfully
     * registered for Tor events, including any desired log events. */
    _torControl->closeTorStdout();
  }

  /* Configure UPnP port forwarding if needed */
  serverSettings.configurePortForwarding();

  /* Check if Tor has a circuit established */
  if (_torControl->isCircuitEstablished()) {
    circuitEstablished();
  }
  /* Check the status of Tor's version */
  if (_torControl->getTorVersion() >= 0x020001)
    checkTorVersion();
  if (_torControl->getTorVersion() >= 0x020102) {
    BootstrapStatus status = _torControl->bootstrapStatus();
    if (status.isValid())
      bootstrapStatusChanged(status);
  }
}

/** Called when Vidalia fails to authenticate to Tor. The failure reason is
 * specified in <b>errmsg</b>. */
void
MainWindow::authenticationFailed(QString errmsg)
{
  bool retry = false;
  
  vWarn("Authentication failed: %1").arg(errmsg);

  /* Parsing log messages is evil, but we're left with little option */
  if (errmsg.contains("Password did not match")) {
    ControlPasswordInputDialog dlg;
    connect(&dlg, SIGNAL(helpRequested(QString)),
            this, SLOT(showHelpDialog(QString)));

    qint64 torPid = 0;

    TorSettings settings;
    QHash<qint64, QString> procs = process_list(settings.getControlPort());
    foreach (qint64 pid, procs.keys()) {
      if (! procs.value(pid).compare("tor", Qt::CaseInsensitive)) {
        torPid = pid;
        break;
      }
    }
    
    dlg.setResetEnabled(torPid > 0);

    int ret = dlg.exec();
    if (ret == QDialogButtonBox::Reset) {
      if (! process_kill(torPid)) {
        VMessageBox::warning(this,
          tr("Password Reset Failed"),
          p(tr("Vidalia tried to restart Tor, but was not "
               "able to. Please check your Task Manager to "
               "ensure there are no other Tor processes running.")),
               VMessageBox::Ok|VMessageBox::Default);
      } else {
        retry = true;
      }
    }
  }
  
  if (_torControl->isRunning())
    if (_isVidaliaRunningTor) 
      stop();
    else
      disconnect();
  if (retry)
    start();
}

/** Called when Tor thinks its version is old or unrecommended, and displays
 * a message notifying the user. */
void
MainWindow::dangerousTorVersion(tc::TorVersionStatus reason,
                                const QString &current,
                                const QStringList &recommended)
{
  Q_UNUSED(current);
  Q_UNUSED(recommended);

  if (reason == tc::ObsoleteTorVersion
        || reason == tc::UnrecommendedTorVersion)
    displayTorVersionWarning();
}

/** Called when Tor thinks its version is old or unrecommended, and displays a
 * message notifying the user. */
void
MainWindow::displayTorVersionWarning()
{
  static bool alreadyWarned = false;

  if (!alreadyWarned) {
#if !defined(USE_AUTOUPDATE)
    QString website = "https://www.torproject.org/";
# if QT_VERSION >= 0x040200
    website = QString("<a href=\"%1\">%1</a>").arg(website);
# endif

    VMessageBox::information(this, tr("Tor Update Available"),
      p(tr("The currently installed version of Tor is out of date or no longer "
           "recommended. Please visit the Tor website to download the latest "
           "version.")) + p(tr("Tor website: %1").arg(website)),
      VMessageBox::Ok);
#else
    int ret = VMessageBox::information(this,
                tr("Tor Update Available"),
                p(tr("The currently installed version of Tor is out of date "
                     "or no longer recommended."))
                  + p(tr("Would you like to check if a newer package is "
                         "available for installation?")),
                VMessageBox::Yes|VMessageBox::Default,
                VMessageBox::No|VMessageBox::Escape);

    if (ret == VMessageBox::Yes)
      checkForUpdatesWithUi();
#endif
    alreadyWarned = true;
  }
}

/** Called when Tor's bootstrapping status changes. <b>bse</b> represents
 * Tor's current estimate of its bootstrapping progress. */
void
MainWindow::bootstrapStatusChanged(const BootstrapStatus &bs)
{
  int percentComplete = STARTUP_PROGRESS_BOOTSTRAPPING + bs.percentComplete();
  bool warn = (bs.severity() == tc::WarnSeverity && 
               bs.recommendedAction() != BootstrapStatus::RecommendIgnore);

  QString description;
  switch (bs.status()) {
    case BootstrapStatus::ConnectingToDirMirror:
      description = tr("Connecting to a relay directory");
      break;
    case BootstrapStatus::HandshakingWithDirMirror:
    case BootstrapStatus::CreatingOneHopCircuit:
      description = tr("Establishing an encrypted directory connection");
      break;
    case BootstrapStatus::RequestingNetworkStatus:
      description = tr("Retrieving network status");
      break;
    case BootstrapStatus::LoadingNetworkStatus:
      description = tr("Loading network status");
      break;
    case BootstrapStatus::LoadingAuthorityCertificates:
      description = tr("Loading authority certificates");
      break;
    case BootstrapStatus::RequestingDescriptors:
      description = tr("Requesting relay information");
      break;
    case BootstrapStatus::LoadingDescriptors:
      description = tr("Loading relay information");
      break;
    case BootstrapStatus::ConnectingToEntryGuard:
      description = tr("Connecting to the Tor network");
      break;
    case BootstrapStatus::HandshakingWithEntryGuard:
    case BootstrapStatus::EstablishingCircuit:
      description = tr("Establishing a Tor circuit");
      break;
    case BootstrapStatus::BootstrappingDone:
      description = tr("Connected to the Tor network!");
      warn = false; /* probably false anyway */
      break;
    default:
      description = tr("Unrecognized startup status");
  }
  if (warn) {
    QString reason;
    /* Is it really a good idea to translate these? */
    switch (bs.reason()) {
      case tc::MiscellaneousReason:
        reason = tr("miscellaneous");
        break;
      case tc::IdentityMismatch:
        reason = tr("identity mismatch");
        break;
      case tc::ConnectionDone:
        reason = tr("done");
        break;
      case tc::ConnectionRefused:
        reason = tr("connection refused");
        break;
      case tc::ConnectionTimeout:
        reason = tr("connection timeout");
        break;
      case tc::ConnectionIoError:
        reason = tr("read/write error");
        break;
      case tc::NoRouteToHost:
        reason = tr("no route to host");
        break;
      case tc::ResourceLimitReached:
        reason = tr("insufficient resources");
        break;
      default:
        reason = tr("unknown");
    }
    description += tr(" failed (%1)").arg(reason);
  }
  setStartupProgress(percentComplete, description);
}

/** Called when Tor has successfully established a circuit. */
void
MainWindow::circuitEstablished()
{
  updateTorStatus(CircuitEstablished);
  // TODO: fix hardcoded total length
  setStartupProgress(130,
                     tr("Connected to the Tor network!"));

#if defined(USE_AUTOUPDATE)
  VidaliaSettings settings;
  if (settings.isAutoUpdateEnabled()) {
    QDateTime lastCheckedAt = settings.lastCheckedForUpdates();
    if (UpdateProcess::shouldCheckForUpdates(lastCheckedAt)) {
      /* Initiate a background check for updates now */
      _updateTimer.stop();
      checkForUpdates();
    }
  }
#endif
}

/** Called when Tor thinks the user has tried to connect to a port that
 * typically is used for unencrypted applications. Warns the user and allows
 * them to ignore future warnings on <b>port</b>. It is possible that Tor
 * will produce multiple asynchronous status events warning of dangerous ports
 * while the message box is displayed (for example, while the user is away
 * from the keyboard), so subsequent messages will be discarded until the
 * first message box is dismissed. */
void
MainWindow::warnDangerousPort(quint16 port, bool rejected)
{
  static QMessageBox *dlg = 0;

  /* Don't display another message box until the first one is dismissed */
  if (dlg)
    return;

  QString application;
  switch (port) {
    case  23:
     application = tr("(probably Telnet)");
     break;

    case 109:
    case 110:
    case 143:
      application = tr("(probably an email client)"); 
      break;

    default:
      application = "";
  }

  QString text = tr("One of your applications %1 appears to be making a "
                    "potentially unencrypted and unsafe connection to port %2.")
                    .arg(application).arg(port);

  QString extraText = p(tr("Anything sent over this connection could be "
                           "monitored. Please check your application's "
                           "configuration and use only encrypted protocols, "
                           "such as SSL, if possible."));
  if (rejected) {
    extraText.append(p(tr("Tor has automatically closed your connection in "
                          "order to protect your anonymity.")));
  }

  dlg = new QMessageBox(QMessageBox::Warning,
                        tr("Potentially Unsafe Connection"), text,
                        QMessageBox::Ok | QMessageBox::Ignore);
  dlg->setInformativeText(extraText);
  dlg->setDefaultButton(QMessageBox::Ok);
  dlg->setEscapeButton(QMessageBox::Ok);

  int ret = dlg->exec();
  if (ret == QMessageBox::Ignore) {
    TorControl *tc = Vidalia::torControl();
    TorSettings settings;
    QStringList portList;
    QList<quint16> ports;
    int idx;

    ports = settings.getWarnPlaintextPorts();
    idx   = ports.indexOf(port);
    if (idx >= 0) {
      ports.removeAt(idx);
      settings.setWarnPlaintextPorts(ports);

      foreach (quint16 port, ports) {
        portList << QString::number(port);
      }
      tc->setConf("WarnPlaintextPorts", portList.join(","));
      portList.clear();
    }

    ports = settings.getRejectPlaintextPorts();
    idx   = ports.indexOf(port);
    if (idx >= 0) {
      ports.removeAt(idx);
      settings.setRejectPlaintextPorts(ports);

      foreach (quint16 port, ports) {
        portList << QString::number(port);
      }
      tc->setConf("RejectPlaintextPorts", portList.join(","));
    }
  }
  delete dlg;
  dlg = 0;
}

/** Attempts to authenticate to Tor's control port, depending on the
 * authentication method specified in TorSettings::getAuthenticationMethod().
 */
bool
MainWindow::authenticate()
{
  TorSettings::AuthenticationMethod authMethod;
  TorSettings settings;
  ProtocolInfo pi;
  
  updateTorStatus(Authenticating);
  setStartupProgress(STARTUP_PROGRESS_AUTHENTICATING,
                     tr("Authenticating to Tor"));

  authMethod = settings.getAuthenticationMethod(); 
  pi = _torControl->protocolInfo();
  QStringList authMethods;
  if (!pi.isEmpty()) {
    authMethods = pi.authMethods();
    if (authMethods.contains("COOKIE"))
      authMethod = TorSettings::CookieAuth;
    else if (authMethods.contains("HASHEDPASSWORD"))
      authMethod = TorSettings::PasswordAuth;
    else if (authMethods.contains("NULL"))
      authMethod = TorSettings::NullAuth;
  }

  if (authMethod == TorSettings::CookieAuth) {
    if(!tryCookie(pi)) {
      if(authMethods.contains("HASHEDPASSWORD") and !tryHashed()) {
        goto cancel;
      } else {
        return true;
      }
    } else {
      return true;
    }
  } else if (authMethod == TorSettings::PasswordAuth) {
    return tryHashed();
  }
  /* No authentication. Send an empty password. */
  vNotice("Authenticating using 'null' authentication.");
  return _torControl->authenticate(QString(""));

cancel:
  vWarn("Cancelling control authentication attempt.");
  if (_isVidaliaRunningTor)
    stop();
  else
    disconnect();
  return false;
}

bool
MainWindow::tryCookie(const ProtocolInfo &pi)
{
  TorSettings settings;
  /* Try to load an auth cookie and send it to Tor */
  QByteArray cookie = loadControlCookie(pi.cookieAuthFile());
  while (cookie.isEmpty()) {
    /* Prompt the user to find their control_auth_cookie */
    int ret = VMessageBox::question(this,
                                    tr("Cookie Authentication Required"),
                                    p(tr("The Tor software requires Vidalia to send the "
                                         "contents of an authentication cookie, but Vidalia "
                                         "was unable to find one."))
                                    + p(tr("Would you like to browse for the file "
                                           "'control_auth_cookie' yourself?")),
                                    VMessageBox::Browse|VMessageBox::Default,
                                    VMessageBox::Cancel|VMessageBox::Escape);
      
    if (ret == VMessageBox::Cancel)
      return false;
    QString cookieDir = QFileDialog::getOpenFileName(this,
                                                     tr("Data Directory"),
                                                     settings.getDataDirectory(),
                                                     tr("Control Cookie (control_auth_cookie)"));
    if (cookieDir.isEmpty())
      return false;
    cookieDir = QFileInfo(cookieDir).absolutePath();
    cookie = loadControlCookie(cookieDir);
  }
  if(cookie.size() != 32) {
    vWarn(QString("Cookie length has to be exactly 32 bytes long. Found %s bytes")
          .arg(cookie.size()));
    return false;
  }
  vNotice("Authenticating using 'cookie' authentication.");
  return _torControl->authenticate(cookie);
}

bool
MainWindow::tryHashed()
{
  /* Get the control password and send it to Tor */
  vNotice("Authenticating using 'hashed password' authentication.");
  if (_useSavedPassword) {
    TorSettings settings;
    _controlPassword = settings.getControlPassword();
  }

  qputenv("TOR_CONTROL_PASSWD",
          _controlPassword.toAscii().toHex());

  return _torControl->authenticate(_controlPassword);
}

/** Checks the status of the current version of Tor to see if it's old,
 * unrecommended, or obsolete. */
void
MainWindow::checkTorVersion()
{
  VidaliaSettings settings;
  if(settings.skipVersionCheck())
    return;
  QString status;
  if (_torControl->getInfo("status/version/current", status)) {
    if (!status.compare("old", Qt::CaseInsensitive)
          || !status.compare("unrecommended", Qt::CaseInsensitive)
          || !status.compare("obsolete", Qt::CaseInsensitive)) {
      displayTorVersionWarning();
    }
  }
}

/** Searches for and attempts to load the control authentication cookie. This
 * assumes the cookie is named 'control_auth_cookie'. If <b>cookiePath</b> is
 * empty, this method will search some default locations depending on the
 * current platform. <b>cookiePath</b> can point to either a cookie file or a
 * directory containing the cookie file. */
QByteArray
MainWindow::loadControlCookie(QString cookiePath)
{
  QFile authCookie;
  QStringList pathList;

  if (!cookiePath.isEmpty()) {
    pathList << cookiePath;
  } else {
    /* Try some default locations */
    TorSettings settings;
    QString dataDir = settings.getDataDirectory();
    if (!dataDir.isEmpty())
      pathList << dataDir;
      
#if defined(Q_WS_WIN)
    pathList << expand_filename("%APPDATA%\\Tor");
#else
    pathList << expand_filename("~/.tor");
#endif
  }
  
  /* Search for the cookie file */
  foreach (QString path, pathList) {
    QString cookieFile = QFileInfo(path).isFile() ?
                          path : path + "/control_auth_cookie";
    vDebug("Checking for authentication cookie in '%1'").arg(cookieFile);
    if (!QFileInfo(cookieFile).exists())
      continue;
    
    authCookie.setFileName(cookieFile);
    if (authCookie.open(QIODevice::ReadOnly)) {
      vInfo("Reading authentication cookie from '%1'").arg(cookieFile);
      return authCookie.readAll();
    } else {
      vWarn("Couldn't open cookie file '%1': %2")
        .arg(cookieFile).arg(authCookie.errorString());
    }
  }
  vWarn("Couldn't find a readable authentication cookie.");
  return QByteArray();
}

/** Updates the UI to reflect Tor's current <b>status</b>. Returns the
 * previously set TorStatus value.*/
MainWindow::TorStatus
MainWindow::updateTorStatus(TorStatus status)
{
  QString statusText, actionText;
  QString trayIconFile, statusIconFile;
  TorStatus prevStatus = _status;
 
  vNotice("Tor status changed from '%1' to '%2'.")
    .arg(toString(prevStatus)).arg(toString(status));
  _status = status;

  if (status == Stopped) {
      statusText = tr("Tor is not running");
      actionText = tr("Start Tor");
      trayIconFile = IMG_TOR_STOPPED;
      statusIconFile = IMG_TOR_STOPPED_48;
      _actionRestartTor->setEnabled(false);
      _actionReloadConfig->setEnabled(false);
      _actionStartStopTor->setEnabled(true);
      _actionStartStopTor->setEnabled(true);
      _actionStartStopTor->setIcon(QIcon(IMG_START_TOR_16));
      _actionStartStopTor->setText(actionText);

      /* XXX: This might need to be smarter if we ever start connecting other
       * slots to these triggered() and clicked() signals. */
      QObject::disconnect(_actionStartStopTor, SIGNAL(triggered()), this, 0);
      connect(_actionStartStopTor, SIGNAL(triggered()), this, SLOT(start()));
      setStartupProgressVisible(false);
  } else if (status == Stopping) {
      _actionRestartTor->setEnabled(false);
      _actionReloadConfig->setEnabled(false);
      if (_delayedShutdownStarted) {
        statusText = tr("Your relay is shutting down.\n" 
                        "Click 'Stop' again to stop your relay now.");
      } else {
        statusText = tr("Tor is shutting down");
      }
      trayIconFile = IMG_TOR_STOPPING;
      statusIconFile = IMG_TOR_STOPPING_48;
      
  } else if (status == Started) {
      actionText = tr("Stop Tor");
      _actionRestartTor->setEnabled(true);
      _actionReloadConfig->setEnabled(true);
      _actionStartStopTor->setEnabled(true);
      _actionStartStopTor->setIcon(QIcon(IMG_STOP_TOR_16));
      _actionStartStopTor->setText(actionText);
            
      /* XXX: This might need to be smarter if we ever start connecting other
       * slots to these triggered() and clicked() signals. */
      QObject::disconnect(_actionStartStopTor, SIGNAL(triggered()), this, 0);
      connect(_actionStartStopTor, SIGNAL(triggered()), this, SLOT(stop()));
  } else if (status == Starting)  {
      statusText = tr("Starting the Tor software");
      trayIconFile = IMG_TOR_STARTING;
      statusIconFile = IMG_TOR_STARTING_48;
      _actionStartStopTor->setEnabled(false);
      _actionRestartTor->setEnabled(false);
      _actionReloadConfig->setEnabled(false);
      setStartupProgressVisible(true);
      setStartupProgress(STARTUP_PROGRESS_STARTING, statusText);
  } else if (status == CircuitEstablished) {
      statusText = tr("Connected to the Tor network!");
      trayIconFile = IMG_TOR_RUNNING;
      statusIconFile = IMG_TOR_RUNNING_48;
      setStartupProgressVisible(false);
  }

  /* Update the tray icon */
  if (!trayIconFile.isEmpty()) {
    setTrayIcon(trayIconFile);
  }
  /* Update the status banner on the control panel */
  if (!statusIconFile.isEmpty())
    _statusTab.setTorStatus(QPixmap(statusIconFile));
  if (!statusText.isEmpty()) {
    _trayIcon.setToolTip(statusText);
    _statusTab.setTorStatus(statusText);
  }
  return prevStatus;
}

/** Sets the current tray or dock icon image to <b>iconFile</b>. */
void
MainWindow::setTrayIcon(const QString &iconFile)
{
#if defined(Q_WS_MAC)
  VidaliaSettings settings;
  QApplication::setWindowIcon(QPixmap(iconFile));

  /* only display tray icon if icon preference is not set to "Dock Only" */
  if (settings.getIconPref() != VidaliaSettings::Dock)
    _trayIcon.setIcon(QIcon(iconFile));
#else
  /* always display tray icon for other platforms */
  _trayIcon.setIcon(QIcon(iconFile));
#endif
}

/** Converts a TorStatus enum value to a string for debug logging purposes. */
QString
MainWindow::toString(TorStatus status)
{
  switch (status) {
    /* These strings only appear in debug logs, so they should not be
     * translated. */
    case Unset:     return "Unset";
    case Stopping:  return "Stopping";
    case Stopped:   return "Stopped";
    case Starting:  return "Starting";
    case Started:   return "Started";
    case Authenticating:  return "Authenticating";
    case Authenticated:   return "Authenticated";
    case CircuitEstablished: return "Circuit Established";
    default: break;
  }
  return "Unknown";
}

/** Called when the user changes a setting that needs Tor restarting */
void
MainWindow::restart()
{
  if(stop()) {
    start();
  }
}

/** Sends a sighup signal to Tor on platforms other than Windows */
void
MainWindow::sighup()
{
  bool rc;
  QString errmsg;
  rc = _torControl->signal(TorSignal::Reload, &errmsg);

  if (!rc) {
    int response = VMessageBox::warning(this, tr("Error reloading configuration"),
                     p(tr("Vidalia was unable to reload Tor's configuration.")) 
                       + p(errmsg),
                     VMessageBox::Ok);
  }
}

/** Sets the visibility of the startup status description and progress bar to
 * <b>visible</b>. */
void
MainWindow::setStartupProgressVisible(bool visible)
{
  _statusTab.setTorStatusVisible(!visible);
  _statusTab.setProgressVisible(visible);
}

/** Sets the progress bar completion value to <b>progressValue</b> and sets
 * the status text to <b>description</b>. */
void
MainWindow::setStartupProgress(int progressValue,
                               const QString &description)
{
  _statusTab.setProgress(progressValue, description);
  _trayIcon.setToolTip(description);
}

/** Called when Vidalia wants to disconnect from a Tor it did not start. */
void
MainWindow::disconnect()
{
  _torControl->disconnect();
}

/** Called when the control socket has been disconnected. */
void
MainWindow::disconnected()
{
  if (!_isVidaliaRunningTor) {
    /* If we didn't start our own Tor process, interpret losing the control
     * connection as "Tor is stopped". */
    updateTorStatus(Stopped);
  }
  
  /*XXX We should warn here if we get disconnected when we didn't intend to */
  _actionNewIdentity->setEnabled(false);
  _isVidaliaRunningTor = false;
}

void
MainWindow::showConfigDialog(ConfigDialog::Page page)
{
  ConfigDialog *configDialog = new ConfigDialog();
  connect(configDialog, SIGNAL(helpRequested(QString)),
          this, SLOT(showHelpDialog(QString)));
  configDialog->showWindow(page);
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
    _actionNewIdentity->setEnabled(false);
    QTimer::singleShot(MIN_NEWIDENTITY_INTERVAL, 
                       this, SLOT(enableNewIdentity()));

    if (QSystemTrayIcon::supportsMessages())
      _trayIcon.showMessage(title, message, QSystemTrayIcon::Information);
    else
      VMessageBox::information(this, title, message, VMessageBox::Ok);
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
    _actionNewIdentity->setEnabled(true);
  }
}

void
MainWindow::handleCloseTab(int index)
{
  /* If the tab that's going to be closed is the status one
   * don't do it */
  if(index == 0)
    return;

  delTab(index);
}

void
MainWindow::attachTab()
{
  QAction *act = qobject_cast<QAction *>(sender());
  VidaliaTab *tab = qobject_cast<VidaliaTab *>(act->parent());
  _detachedTabMap.removeAll(tab->getTitle());
  tab->setParent(ui.tabWidget);
  addTab(tab);
  _reattachMenu.removeAction(act);
  if(_reattachMenu.actions().size() < 1) {
    _reattachMenu.addAction(_dummy);
    _dummy->setEnabled(false);
  }
}

void
MainWindow::detachTab()
{
  VAttachButton *but = qobject_cast<VAttachButton *>(sender());
  VidaliaTab *tab = but->getTab();
  int index = ui.tabWidget->indexOf(tab);

  ui.tabWidget->removeTab(index);
  tab->setParent(0);
  tab->show();

  QString key = _tabMap.at(index);
  _tabMap.removeAll(key);
  _detachedTabMap << key;

  QAction *act = new QAction(tab->getTitle(), tab);
  connect(act, SIGNAL(triggered()), this, SLOT(attachTab()));
  _reattachMenu.addAction(act);
  _reattachMenu.removeAction(_dummy);
}

void
MainWindow::handleAttachedClose()
{
  VidaliaTab *tab = qobject_cast<VidaliaTab *>(sender());
  int index = ui.tabWidget->indexOf(tab);
  if(index < 0) {
    QAction *act = 0;
    foreach(QAction *tmpAct, _reattachMenu.actions()) {
      if(tmpAct->parent() == tab) {
        act = tmpAct;
        break;
      }
    }

    if(act != 0)
      _reattachMenu.removeAction(act);

    _detachedTabMap.removeAll(tab->getTitle());

    tab->setParent(ui.tabWidget);
    addTab(tab);

    if(_reattachMenu.actions().size() < 1) {
      _reattachMenu.addAction(_dummy);
      _dummy->setEnabled(false);
    }

    delTab(ui.tabWidget->currentIndex());
  }
}

void
MainWindow::addTab(VidaliaTab *tab)
{
  /** If the tab's already open, display it and delete the
   * instanse passed */
  if(_tabMap.contains(tab->getTitle())) {
    ui.tabWidget->setCurrentIndex(_tabMap.indexOf(tab->getTitle()));

    /** If we are trying to open the exact same tab twice
     * don't do anything */
    if(tab == ui.tabWidget->widget(_tabMap.indexOf(tab->getTitle())))
      return;

    /** Exception for tabs that need to be always created */
    if (tab != _messageLog && 
        tab != &_statusTab && 
        tab != &_netViewer &&
        tab != _graph)
      tab->deleteLater();
    return;
  }

  VAttachButton *atb = new VAttachButton();

  ui.tabWidget->addTab(tab, tab->getTitle());
  int pos = ui.tabWidget->count() - 1;
  ui.tabWidget->setCurrentIndex(pos);

  atb->setTab(tab);
#if defined(Q_WS_MAC)
  ui.tabWidget->setTabButton(pos, QTabBar::RightSide, atb);
#else
  ui.tabWidget->setTabButton(pos, QTabBar::LeftSide, atb);
#endif

  connect(tab, SIGNAL(closeTab()),
          this, SLOT(handleAttachedClose()));

  connect(atb, SIGNAL(attachTab()),
          this, SLOT(attachTab()));
  connect(atb, SIGNAL(detachTab()),
          this, SLOT(detachTab()));
  /** The new tab is added to the last position */
  _tabMap << tab->getTitle();
  connect(tab, SIGNAL(helpRequested(QString)),
          this, SLOT(showHelpDialog(QString)));
  if(!isVisible())
    setVisible(true);
}

void
MainWindow::delTab(int index)
{
  if(index == -1)
    index = ui.tabWidget->currentIndex();

  VidaliaTab *tab = qobject_cast<VidaliaTab*>(ui.tabWidget->widget(index));
  // if it isn't one of the tabs that's supposed to be open at every moment
  if (tab != _messageLog && 
      tab != &_statusTab && 
      tab != &_netViewer &&
      tab != _graph) {
    QObject::disconnect(ui.tabWidget->widget(index), 0, 0, 0);
    tab->deleteLater();
  }
  ui.tabWidget->removeTab(index);
  QString key = _tabMap.at(index);
  _tabMap.removeAll(key);
}

void
MainWindow::showStatusTab()
{
  addTab(&_statusTab);
}

void 
MainWindow::showMessageLogTab()
{
  addTab(_messageLog);
}

void 
MainWindow::showBandwidthTab()
{
  addTab(_graph);
}

/** Creates and displays Vidalia's About dialog. */
void
MainWindow::showAboutDialog()
{
  AboutDialog dlg(this);
  dlg.exec();
}

/** Displays the help browser and displays the most recently viewed help
 * topic. */
void
MainWindow::showHelpDialog()
{
  showHelpDialog(QString());
}

/**< Shows the help browser and displays the given help <b>topic</b>. */
void
MainWindow::showHelpDialog(const QString &topic)
{
  static HelpBrowser *helpBrowser = 0;
  if (!helpBrowser)
    helpBrowser = new HelpBrowser(this);
  helpBrowser->showWindow(topic);
}

void 
MainWindow::showNetViewerTab()
{
  addTab(&_netViewer);
}

void
MainWindow::showDebugDialog()
{
  DebugDialog dlg;
  dlg.exec();
}

#if defined(USE_MINIUPNPC)
/** Called when a UPnP error occurs. */
void
MainWindow::upnpError(UPNPControl::UPNPError error)
{
  Q_UNUSED(error);

#if 0
  /* XXX: Is there a better way to do this? Currently, this could get called
   * if there is an error when testing UPnP support, and again when attempting
   * to reset the UPnP state when the test dialog is closed. The user would
   * not be amused with all the warning dialogs. */

  VMessageBox::warning(this,
    tr("Port Forwarding Failed"),
    p(tr("Vidalia was unable to configure automatic port forwarding."))
      + p(UPNPControl::Instance()->errorString()),
    VMessageBox::Ok);
#endif
}
#endif

#if defined(USE_AUTOUPDATE)
/** Called when the user clicks the 'Check Now' button in the General
 * settings page. */
void
MainWindow::checkForUpdatesWithUi()
{
  checkForUpdates(true);
}

/** Called when the update interval timer expires, notifying Vidalia that
 * we should check for updates again. */
void
MainWindow::checkForUpdates(bool showProgress)
{
  VidaliaSettings settings;

  if (_updateProcess.isRunning()) {
    if (showProgress) {
      /* A check for updates is already in progress, so just bring the update
       * progress dialog into focus.
       */
      _updateProgressDialog.show();
    }
  } else {
    /* If Tor is running and bootstrapped, then use Tor to check for updates */
    if (_torControl->isRunning() && _torControl->circuitEstablished())
      _updateProcess.setSocksPort(_torControl->getSocksPort());
    else
      _updateProcess.setSocksPort(0);

    /* Initialize the UpdateProgressDialog and display it, if necessary. */
    _updateProgressDialog.setStatus(UpdateProgressDialog::CheckingForUpdates);
    if (showProgress)
      _updateProgressDialog.show();

    /* Initiate a check for available software updates. This check will
     * be done in the background, notifying the user only if there are
     * updates to be installed.
     */
    _updateProcess.checkForUpdates(UpdateProcess::TorBundleInfo);

    /* Remember when we last checked for software updates */
    settings.setLastCheckedForUpdates(QDateTime::currentDateTime().toUTC());

    /* Restart the "Check for Updates" timer */
    _updateTimer.start(UpdateProcess::checkForUpdatesInterval() * 1000);
  }
}

/** Called when the check for software updates fails. */
void
MainWindow::checkForUpdatesFailed(const QString &errmsg)
{
  if (_updateProgressDialog.isVisible()) {
    _updateProgressDialog.hide();
    VMessageBox::warning(this, tr("Update Failed"), errmsg,
                         VMessageBox::Ok);
  }
}

/** Called when there is an update available for installation. */
void
MainWindow::updatesAvailable(UpdateProcess::BundleInfo bi,
                             const PackageList &packageList)
{
  vInfo("%1 software update(s) available").arg(packageList.size());
  if (packageList.size() > 0) {
    UpdatesAvailableDialog dlg(packageList, &_updateProgressDialog);

    switch (dlg.exec()) {
      case UpdatesAvailableDialog::InstallUpdatesNow:
        installUpdates(bi);
        break;

      default:
        _updateProgressDialog.hide();
        break;
    }
  } else {
    if (_updateProgressDialog.isVisible()) {
      _updateProgressDialog.hide();
      VMessageBox::information(this, tr("Your software is up to date"),
                               tr("There are no new Tor software packages "
                                  "available for your computer at this time."),
                               VMessageBox::Ok);
    }
  }
}

/** Stops Tor (if necessary), installs any available for <b>bi</b>, and
 * restarts Tor (if necessary). */
void
MainWindow::installUpdates(UpdateProcess::BundleInfo bi)
{
  _updateProgressDialog.setStatus(UpdateProgressDialog::InstallingUpdates);
  _updateProgressDialog.show();

  if (_isVidaliaRunningTor) {
    _restartTorAfterUpgrade = true;
    _isIntentionalExit = true;
    _torControl->stop();
  } else {
    _restartTorAfterUpgrade = false;
  }
  _updateProcess.installUpdates(bi);
}

/** Called when all <b>numUpdates</b> software updates have been installed
 * successfully. */
void
MainWindow::updatesInstalled(int numUpdates)
{
  _updateProgressDialog.setStatus(UpdateProgressDialog::UpdatesInstalled);
  _updateProgressDialog.show();

  if (_restartTorAfterUpgrade)
    start();
}

/** Called when an update fails to install. <b>errmsg</b> contains details
 * about the failure. */
void
MainWindow::installUpdatesFailed(const QString &errmsg)
{
  _updateProgressDialog.hide();

  VMessageBox::warning(this, tr("Installation Failed"),
                       p(tr("Vidalia was unable to install your software updates."))
                         + p(tr("The following error occurred:")) 
                         + p(errmsg),
                       VMessageBox::Ok);

  if (_restartTorAfterUpgrade)
    start();
}

#endif

