/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file mainwindow.cpp
** \version $Id$
** \brief Main (hidden) window. Creates tray menu and child windows
**
** Implements the main window. The main window is a hidden window that serves
** as the parent of the tray icon and popup menu, as well as other application
** dialogs.
*/

#include <QtGui>
#include <QTimer>
#include <QTextStream>
#include <vidalia.h>
#include <file.h>
#include <html.h>
#include <stringutil.h>
#include <net.h>
#include <clientstatusevent.h>
#include <dangerousversionevent.h>
#include <vmessagebox.h>
#include <procutil.h>

#include "mainwindow.h"
#include "controlpasswordinputdialog.h"

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
#define IMG_TOR_STOPPED_48   ":/images/48x48/status-red.png"
#define IMG_TOR_RUNNING_48   ":/images/48x48/status-green.png"
#define IMG_TOR_STARTING_48  ":/images/48x48/status-orange.png"
#define IMG_TOR_STOPPING_48  ":/images/48x48/status-orange.png"

/* Decide which of our four sets of tray icons to use. */
#if defined(Q_WS_WIN)
/* QSystemTrayIcon on Windows wants 16x16 .png files */
#define IMG_TOR_STOPPED  ":/images/16x16/status-red.png"
#define IMG_TOR_RUNNING  ":/images/16x16/status-green.png"
#define IMG_TOR_STARTING ":/images/16x16/status-orange.png"
#define IMG_TOR_STOPPING ":/images/16x16/status-orange.png"
#elif defined(Q_WS_MAC)
/* On Mac, we always go straight to Carbon to load our dock images 
 * from .icns files */
#define IMG_TOR_STOPPED    "status-red"
#define IMG_TOR_RUNNING    "status-green"
#define IMG_TOR_STARTING   "status-orange"
#define IMG_TOR_STOPPING   "status-orange"
#else
/* On X11, we just use always the 22x22 .png files */
#define IMG_TOR_STOPPED    ":/images/22x22/status-red.png"
#define IMG_TOR_RUNNING    ":/images/22x22/status-green.png"
#define IMG_TOR_STARTING   ":/images/22x22/status-orange.png"
#define IMG_TOR_STOPPING   ":/images/22x22/status-orange.png"
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
  VidaliaSettings settings;

  ui.setupUi(this);

  /* Pressing 'Esc' or 'Ctrl+W' will close the window */
  Vidalia::createShortcut("Ctrl+W", this, ui.btnHide, SLOT(click()));
  Vidalia::createShortcut("Esc", this, ui.btnHide, SLOT(click()));

  /* Create all the dialogs of which we only want one instance */
  _messageLog     = new MessageLog();
  _bandwidthGraph = new BandwidthGraph();
  _netViewer      = new NetViewer();
  _configDialog   = new ConfigDialog();
  _menuBar        = 0;
  connect(_messageLog, SIGNAL(helpRequested(QString)),
          this, SLOT(showHelpDialog(QString)));
  connect(_netViewer, SIGNAL(helpRequested(QString)),
          this, SLOT(showHelpDialog(QString)));
  connect(_configDialog, SIGNAL(helpRequested(QString)),
          this, SLOT(showHelpDialog(QString)));

  /* Create the actions that will go in the tray menu */
  createActions();
  /* Creates a tray icon with a context menu and adds it to the system's
   * notification area. */
  createTrayIcon();
  /* Start with Tor initially stopped */
  _status = Unset;
  updateTorStatus(Stopped);
  
  /* Create a new TorControl object, used to communicate with Tor */
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
  connect(_torControl, SIGNAL(authenticated()), this, SLOT(authenticated()));
  connect(_torControl, SIGNAL(authenticationFailed(QString)),
                 this,   SLOT(authenticationFailed(QString)));
  _torControl->setEvent(TorEvents::ClientStatus,  this, true);
  _torControl->setEvent(TorEvents::GeneralStatus, this, true);

  /* Create a new HelperProcess object, used to start the web browser */
  _browserProcess = new HelperProcess(this);
  connect(_browserProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
           this, SLOT(onSubprocessFinished(int, QProcess::ExitStatus)));
  connect(_browserProcess, SIGNAL(startFailed(QString)),
           this, SLOT(onBrowserFailed(QString)));
  _browserProcess->setEnvironment(QProcess::systemEnvironment() << "TZ=UTC");

  /* Create a new HelperProcess object, used to start the IM client */
  _imProcess = new HelperProcess(this);
  connect(_imProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
           this, SLOT(onSubprocessFinished(int, QProcess::ExitStatus)));
  connect(_imProcess, SIGNAL(startFailed(QString)),
           this, SLOT(onIMFailed(QString)));

  /* Create a new HelperProcess object, used to start the proxy server */
  _proxyProcess = new HelperProcess(this);
  connect(_proxyProcess, SIGNAL(startFailed(QString)),
           this, SLOT(onProxyFailed(QString)));

  /* Catch signals when the application is running or shutting down */
  connect(vApp, SIGNAL(running()), this, SLOT(running()));
  connect(vApp, SIGNAL(shutdown()), this, SLOT(shutdown()));

#if defined(USE_MINIUPNPC)
  /* Catch UPnP-related signals */
  connect(UPNPControl::instance(), SIGNAL(error(UPNPControl::UPNPError)),
         this, SLOT(upnpError(UPNPControl::UPNPError)));
#endif

  if (TrayIcon::isTrayIconSupported()) {
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
  delete _bandwidthGraph;
  delete _netViewer;
  delete _configDialog;
}

void
MainWindow::retranslateUi()
{
  ui.retranslateUi(this);
  updateTorStatus(_status);

  _bandwidthAct->setText(tr("Bandwidth Graph"));
  _messageAct->setText(tr("Message Log"));
  _networkAct->setText(tr("Network Map"));
  _controlPanelAct->setText(tr("Control Panel"));
  _helpAct->setText(tr("Help"));
  _newIdentityAct->setText(tr("New Identity"));

#if !defined(Q_WS_MAC)
  _aboutAct->setText(tr("About"));
  _configAct->setText(tr("Settings"));
  _exitAct->setText(tr("Exit"));
#else
  createMenuBar();
#endif
}

/** Catches and processes Tor client and general status events. */
void
MainWindow::customEvent(QEvent *event)
{
  if (event->type() == CustomEventType::ClientStatusEvent) {
    ClientStatusEvent *cse = dynamic_cast<ClientStatusEvent *>(event);
    if (!cse)
      return;

    if (cse->status() == ClientStatusEvent::CircuitEstablished) {
      circuitEstablished();
      cse->accept();
    } else if (cse->status() == ClientStatusEvent::Bootstrap) {
      BootstrapStatusEvent *bse = dynamic_cast<BootstrapStatusEvent *>(cse);
      if (bse)
        bootstrapStatusChanged(bse->status());
      cse->accept();
    }
  } else if (event->type() == CustomEventType::GeneralStatusEvent) {
    GeneralStatusEvent *gse = dynamic_cast<GeneralStatusEvent *>(event);
    if (!gse)
      return;

    if (gse->status() == GeneralStatusEvent::DangerousTorVersion) {
      DangerousVersionEvent *dve = dynamic_cast<DangerousVersionEvent *>(gse);
      if (dve && (dve->reason() == DangerousVersionEvent::ObsoleteVersion
           || dve->reason() == DangerousVersionEvent::UnrecommendedVersion)) {
        dangerousTorVersion();
      }
      gse->accept();
    }
  }
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

  /* Start the proxy server, if configured */
  if (settings.runProxyAtStart())
    startProxy();
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

  /* Disable port forwarding */
  ServerSettings settings(_torControl);
  settings.cleanupPortForwarding();

  if (_proxyProcess->state() != QProcess::NotRunning) {
    /* Close the proxy server (Polipo ignores the WM_CLOSE event sent by
     * terminate() so we have to kill() it) */
    _proxyProcess->kill();
  }

  /* Disconnect all of the TorControl object's signals */
  QObject::disconnect(_torControl, 0, 0, 0);

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
        QObject::disconnect(_torControl, SIGNAL(stopped()), this, SLOT(shutdown()));
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
  _startStopAct = new QAction(tr("Start Tor"), this);
  connect(_startStopAct, SIGNAL(triggered()), this, SLOT(start()));

  _exitAct = new QAction(tr("Exit"), this);
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _bandwidthAct = new QAction(tr("Bandwidth Graph"), this);
  connect(_bandwidthAct, SIGNAL(triggered()), 
          _bandwidthGraph, SLOT(showWindow()));
  connect(ui.lblBandwidthGraph, SIGNAL(clicked()),
          _bandwidthGraph, SLOT(showWindow()));

  _messageAct = new QAction(tr("Message Log"), this);
  connect(_messageAct, SIGNAL(triggered()),
          _messageLog, SLOT(showWindow()));
  connect(ui.lblMessageLog, SIGNAL(clicked()),
          _messageLog, SLOT(showWindow()));

  _networkAct = new QAction(tr("Network Map"), this);
  connect(_networkAct, SIGNAL(triggered()), 
          _netViewer, SLOT(showWindow()));
  connect(ui.lblViewNetwork, SIGNAL(clicked()),
          _netViewer, SLOT(showWindow()));

  _controlPanelAct = new QAction(tr("Control Panel"), this);
  connect(_controlPanelAct, SIGNAL(triggered()), this, SLOT(show()));

  _configAct = new QAction(tr("Settings"), this);
  connect(_configAct, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
  
  _aboutAct = new QAction(tr("About"), this);
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

  _helpAct = new QAction(tr("Help"), this);
  connect(_helpAct, SIGNAL(triggered()), this, SLOT(showHelpDialog()));
  connect(ui.lblHelpBrowser, SIGNAL(clicked()), this, SLOT(showHelpDialog()));

  _newIdentityAct = new QAction(tr("New Identity"), this);
  _newIdentityAct->setEnabled(false);
  connect(_newIdentityAct, SIGNAL(triggered()), this, SLOT(newIdentity()));

#if !defined(Q_WS_MAC)
  /* Don't give the menu items icons on OS X, since they end up in the
   * application menu bar. Menu bar items on OS X typically do not have
   * icons. */
  _startStopAct->setIcon(QIcon(IMG_START_TOR_16));
  _exitAct->setIcon(QIcon(IMG_EXIT));
  _bandwidthAct->setIcon(QIcon(IMG_BWGRAPH));
  _messageAct->setIcon(QIcon(IMG_MESSAGELOG));
  _networkAct->setIcon(QIcon(IMG_NETWORK));
  _controlPanelAct->setIcon(QIcon(IMG_CONTROL_PANEL));
  _configAct->setIcon(QIcon(IMG_CONFIG));
  _aboutAct->setIcon(QIcon(IMG_ABOUT));
  _helpAct->setIcon(QIcon(IMG_HELP));
  _newIdentityAct->setIcon(QIcon(IMG_IDENTITY));
#endif
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
  connect(&_trayIcon, SIGNAL(doubleClicked()), this, SLOT(show()));
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
  menu->addSeparator();
  menu->addAction(_controlPanelAct);
  
#if !defined(Q_WS_MAC)
  /* These aren't added to the dock menu on Mac, since they are in the
   * standard Mac locations in the menu bar. */
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
  _configAct->setText("config");
  _configAct->setMenuRole(QAction::PreferencesRole);
  _aboutAct->setText("about");
  _aboutAct->setMenuRole(QAction::AboutRole);
  _exitAct->setText("quit");
  _exitAct->setMenuRole(QAction::QuitRole);

  /* The File, Help, and Configure menus will get merged into the application
   * menu by Qt. */
  if (_menuBar)
    delete _menuBar;
  _menuBar = new QMenuBar(0);
  QMenu *fileMenu = _menuBar->addMenu("File");
  fileMenu->addAction(_exitAct);
  fileMenu->addAction(_configAct);

  QMenu *torMenu = _menuBar->addMenu(tr("Tor"));
  torMenu->addAction(_startStopAct);
  torMenu->addSeparator();
  torMenu->addAction(_newIdentityAct);

  QMenu *viewMenu = _menuBar->addMenu(tr("View"));
  viewMenu->addAction(_controlPanelAct);
  viewMenu->addSeparator();
  viewMenu->addAction(_bandwidthAct);
  viewMenu->addAction(_messageAct);
  viewMenu->addAction(_networkAct);
  
  QMenu *helpMenu = _menuBar->addMenu(tr("Help"));
  _helpAct->setText(tr("Vidalia Help"));
  helpMenu->addAction(_helpAct);
  helpMenu->addAction(_aboutAct);
#endif
}

/** Starts the web browser and IM client, if appropriately configured */
void
MainWindow::startSubprocesses()
{
  VidaliaSettings settings;
  QString executable = settings.getBrowserExecutable();
  
  if (!executable.isEmpty())
    _browserProcess->start(executable, QStringList());

  executable = settings.getIMExecutable();

  if (!executable.isEmpty())
    _imProcess->start(executable, QStringList());
}

/** Called when browser or IM client have exited */
void
MainWindow::onSubprocessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  Q_UNUSED(exitCode)
  Q_UNUSED(exitStatus)

  /* Get path to browser and IM client */
  VidaliaSettings settings;
  QString browserExecutable = settings.getBrowserExecutable();
  QString imExecutable = settings.getIMExecutable();

  /* A subprocess is finished if it successfully exited or was never asked to start */
  bool browserDone = browserExecutable.isEmpty() || _browserProcess->isDone();
  bool imDone = imExecutable.isEmpty() || _imProcess->isDone();

  /* Exit if both subprocesses are finished */
  if (browserDone && imDone)
    shutdown();
}

/** Called when the web browser failed to start, for example, because the path
 * specified to the web browser executable didn't lead to an executable. */
void
MainWindow::onBrowserFailed(QString errmsg)
{
  Q_UNUSED(errmsg);
 
  /* Display an error message and see if the user wants some help */
  VMessageBox::warning(this, tr("Error starting web browser"),
              tr("Vidalia was unable to start the configured web browser"),
              VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape);
}

/** Called when the IM client failed to start, for example, because the path
 * specified to the IM client executable didn't lead to an executable. */
void
MainWindow::onIMFailed(QString errmsg)
{
  Q_UNUSED(errmsg);
 
  /* Display an error message and see if the user wants some help */
  VMessageBox::warning(this, tr("Error starting IM client"),
              tr("Vidalia was unable to start the configured IM client"),
              VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape);
}

/** Starts the proxy server, if appropriately configured */
void
MainWindow::startProxy()
{
  VidaliaSettings settings;
  QString executable = settings.getProxyExecutable();
  _proxyProcess->start(executable, settings.getProxyExecutableArguments());
}

/** Called when the proxy server fails to start, for example, because
 * the path specified didn't lead to an executable. */
void
MainWindow::onProxyFailed(QString errmsg)
{
  Q_UNUSED(errmsg);
 
  /* Display an error message and see if the user wants some help */
  VMessageBox::warning(this, tr("Error starting proxy server"),
              tr("Vidalia was unable to start the configured proxy server"),
              VMessageBox::Ok|VMessageBox::Default|VMessageBox::Escape);
}

/** Called when Tor's bootstrapping status changes. <b>bse</b> represents
 * Tor's current estimate of its bootstrapping progress. */
void
MainWindow::bootstrapStatusChanged(const BootstrapStatus &bs)
{
  int percentComplete = STARTUP_PROGRESS_BOOTSTRAPPING + bs.percentComplete();
  bool warn = (bs.severity() == tc::SeverityWarn && 
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
      _startStopAct->setEnabled(true);
      _startStopAct->setText(actionText);
      _startStopAct->setIcon(QIcon(IMG_START_TOR_16));
      ui.lblStartStopTor->setEnabled(true);
      ui.lblStartStopTor->setText(actionText);
      ui.lblStartStopTor->setPixmap(QPixmap(IMG_START_TOR_48));
      ui.lblStartStopTor->setStatusTip(actionText);

      /* XXX: This might need to be smarter if we ever start connecting other
       * slots to these triggered() and clicked() signals. */
      QObject::disconnect(_startStopAct, SIGNAL(triggered()), this, 0);
      QObject::disconnect(ui.lblStartStopTor, SIGNAL(clicked()), this, 0);
      connect(_startStopAct, SIGNAL(triggered()), this, SLOT(start()));
      connect(ui.lblStartStopTor, SIGNAL(clicked()), this, SLOT(start()));
      setStartupProgressVisible(false);
  } else if (status == Stopping) {
      if (_delayedShutdownStarted) {
        statusText = tr("Your relay is shutting down.\n" 
                        "Click 'Stop' again to stop your relay now.");
      } else {
        statusText = tr("Tor is shutting down");
      }
      trayIconFile = IMG_TOR_STOPPING;
      statusIconFile = IMG_TOR_STOPPING_48;
      
      ui.lblStartStopTor->setStatusTip(tr("Stop Tor Now"));
  } else if (status == Started) {
      actionText = tr("Stop Tor");
      _startStopAct->setEnabled(true);
      _startStopAct->setText(actionText);
      _startStopAct->setIcon(QIcon(IMG_STOP_TOR_16));
      ui.lblStartStopTor->setEnabled(true);
      ui.lblStartStopTor->setText(actionText);
      ui.lblStartStopTor->setPixmap(QPixmap(IMG_STOP_TOR_48));
      ui.lblStartStopTor->setStatusTip(actionText);
            
      /* XXX: This might need to be smarter if we ever start connecting other
       * slots to these triggered() and clicked() signals. */
      QObject::disconnect(_startStopAct, SIGNAL(triggered()), this, 0);
      QObject::disconnect(ui.lblStartStopTor, SIGNAL(clicked()), this, 0);
      connect(_startStopAct, SIGNAL(triggered()), this, SLOT(stop()));
      connect(ui.lblStartStopTor, SIGNAL(clicked()), this, SLOT(stop()));
  } else if (status == Starting)  {
      statusText = tr("Starting the Tor software");
      trayIconFile = IMG_TOR_STARTING;
      statusIconFile = IMG_TOR_STARTING_48;
      _startStopAct->setEnabled(false);
      ui.lblStartStopTor->setText(tr("Starting Tor"));
      ui.lblStartStopTor->setEnabled(false);
      ui.lblStartStopTor->setStatusTip(statusText);
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
    _trayIcon.setIcon(trayIconFile);
  }
  /* Update the status banner on the control panel */
  if (!statusIconFile.isEmpty())
    ui.lblTorStatusImg->setPixmap(QPixmap(statusIconFile));
  if (!statusText.isEmpty()) {
    _trayIcon.setToolTip(statusText);
    ui.lblTorStatus->setText(statusText);
  }
  return prevStatus;
}

/** Called when the "show on startup" checkbox is toggled. */
void
MainWindow::toggleShowOnStartup(bool checked)
{
  VidaliaSettings settings;
  settings.setShowMainWindowAtStart(checked);
}

/** Sets the visibility of the startup status description and progress bar to
 * <b>visible</b>. */
void
MainWindow::setStartupProgressVisible(bool visible)
{
  /* XXX: We force a repaint() to make sure the progress bar and onion status
   * icon don't overlap briefly. This is pretty hacktastic. */
  if (visible) {
    ui.lblTorStatus->setVisible(false);
    ui.lblTorStatusImg->setVisible(false);
    repaint(ui.grpStatus->rect());
    ui.lblStartupProgress->setVisible(true);
    ui.progressBar->setVisible(true);
  } else {
    ui.lblStartupProgress->setVisible(false);
    ui.progressBar->setVisible(false);
    repaint(ui.grpStatus->rect());
    ui.lblTorStatus->setVisible(true);
    ui.lblTorStatusImg->setVisible(true);
  }
}

/** Sets the progress bar completion value to <b>progressValue</b> and sets
 * the status text to <b>description</b>. */
void
MainWindow::setStartupProgress(int progressValue,
                               const QString &description)
{
  ui.progressBar->setValue(progressValue);
  ui.lblStartupProgress->setText(description);
  _trayIcon.setToolTip(description);
}

/** Attempts to start Tor. If Tor fails to start, then startFailed() will be
 * called with an error message containing the reason. */
void 
MainWindow::start()
{
  TorSettings settings;
  QStringList args;

  updateTorStatus(Starting);

  /* Check if Tor is already running separately */
  if (net_test_connect(settings.getControlAddress(),
                       settings.getControlPort())) {
    started();
    return;
  }

  /* Make sure the torrc we want to use really exists. */
  QString torrc = settings.getTorrc();
  if (!torrc.isEmpty()) {
    if (!QFileInfo(torrc).exists())
      touch_file(torrc, true);
    args << "-f" << torrc;
  }

  /* Specify Tor's data directory, if different from the default */
  QString dataDirectory = settings.getDataDirectory();
  if (!dataDirectory.isEmpty())
    args << "DataDirectory" << expand_filename(dataDirectory);
  
  /* Add the intended control port value */
  quint16 controlPort = settings.getControlPort();
  if (controlPort)
    args << "ControlPort" << QString::number(controlPort);
  
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
  /* Try to connect to Tor's control port */
  _torControl->connect(settings.getControlAddress(),
                       settings.getControlPort());
  setStartupProgress(STARTUP_PROGRESS_CONNECTING, tr("Connecting to Tor"));
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

/** Disconnects the control socket and stops the Tor process. */
bool
MainWindow::stop()
{
  ServerSettings server(_torControl);
  QString errmsg;
  TorStatus prevStatus;
  bool rc;

  /* If we're running a server, give users the option of terminating
   * gracefully so clients have time to find new servers. */
  if (server.isServerEnabled() && !_delayedShutdownStarted) {
    /* Ask the user if they want to shutdown nicely. */
    int response = VMessageBox::question(this, tr("Relaying is Enabled"),
                     tr("You are currently running a relay. "
                        "Terminating your relay will interrupt any "
                        "open connections from clients.\n\n"
                        "Would you like to shutdown gracefully and "
                        "give clients time to find a new relay?"),
                        VMessageBox::Yes|VMessageBox::Default, 
                        VMessageBox::No, 
                        VMessageBox::Cancel|VMessageBox::Escape);
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
        _messageLog->showWindow();  
      else if (ret == VMessageBox::Help)
        showHelpDialog("troubleshooting.torexited");
    }
  }
}

/** Called when the control socket has successfully connected to Tor. */
void
MainWindow::connected()
{
  authenticate();
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
  _newIdentityAct->setEnabled(false);
  ui.lblNewIdentity->setEnabled(false);
  _isVidaliaRunningTor = false;
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
  if (!pi.isEmpty()) {
    QStringList authMethods = pi.authMethods();
    if (authMethods.contains("COOKIE"))
      authMethod = TorSettings::CookieAuth;
    else if (authMethods.contains("HASHEDPASSWORD"))
      authMethod = TorSettings::PasswordAuth;
    else if (authMethods.contains("NULL"))
      authMethod = TorSettings::NullAuth;
  }
  
  if (authMethod == TorSettings::CookieAuth) {
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
        goto cancel;
      QString cookieDir = QFileDialog::getOpenFileName(this,
                            tr("Data Directory"),
                            settings.getDataDirectory(),
                            tr("Control Cookie (control_auth_cookie)"));
      if (cookieDir.isEmpty())
        goto cancel;
      cookieDir = QFileInfo(cookieDir).absolutePath();
      cookie = loadControlCookie(cookieDir);
    }
    vNotice("Authenticating using 'cookie' authentication.");
    return _torControl->authenticate(cookie);
  } else if (authMethod == TorSettings::PasswordAuth) {
    /* Get the control password and send it to Tor */
    vNotice("Authenticating using 'hashed password' authentication.");
    if (_useSavedPassword) {
      TorSettings settings;
      _controlPassword = settings.getControlPassword();
    }
    return _torControl->authenticate(_controlPassword);
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
  _newIdentityAct->setEnabled(true);
  ui.lblNewIdentity->setEnabled(true);

  /* Register for any pertinent asynchronous events. */
  if (!_torControl->setEvents(&errmsg)) {
    VMessageBox::warning(this, tr("Error Registering for Events"),
      p(tr("Vidalia was unable to register for some events. "
           "Many of Vidalia's features may be unavailable."))
         + p(errmsg),
      VMessageBox::Ok);
  }

  /* Configure UPnP port forwarding if needed */
  serverSettings.configurePortForwarding();

  /* Check if Tor has a circuit established */
  if (_torControl->circuitEstablished())
    circuitEstablished();
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

#if defined(Q_OS_WIN32)
    QHash<qint64, QString> procs = process_list();
    foreach (qint64 pid, procs.keys()) {
      if (! procs.value(pid).compare("tor.exe", Qt::CaseInsensitive)) {
        torPid = pid;
        break;
      }
    }
    dlg.setResetEnabled(torPid > 0);
#else
    dlg.setResetEnabled(false);
#endif

    int ret = dlg.exec();
    if (ret == QDialogButtonBox::Ok) {
      if (dlg.isSavePasswordChecked()) {
        TorSettings settings;
        settings.setAuthenticationMethod(TorSettings::PasswordAuth);
        settings.setUseRandomPassword(false);
        settings.setControlPassword(dlg.password());
        _useSavedPassword = true;
      } else {
        _controlPassword = dlg.password();
        _useSavedPassword = false;
      }
      retry = true;
    } else if (ret == QDialogButtonBox::Reset) {
      if (! process_kill(torPid)) {
        VMessageBox::warning(this,
          tr("Password Reset Failed"),
          p(tr("Vidalia tried to reset Tor's control password, but was not "
               "able to restart the Tor software. Please check your Task "
               "Manager to ensure there are no other Tor processes running.")),
               VMessageBox::Ok|VMessageBox::Default);
      } else {
        retry = true;
      }
    }
  } else {
    /* Something else went wrong */
    int ret = VMessageBox::warning(this, 
                tr("Authentication Error"),
                p(tr("Vidalia was unable to authenticate to the Tor software. "
                     "(%1)").arg(errmsg)) + 
                p(tr("Please check your control port authentication "
                     "settings.")),
                VMessageBox::ShowSettings|VMessageBox::Default,
                VMessageBox::Cancel|VMessageBox::Escape);
    
    if (ret == VMessageBox::ShowSettings)
      showConfigDialog(ConfigDialog::Advanced);
  }
  
  if (_torControl->isRunning())
    if (_isVidaliaRunningTor) 
      stop();
    else
      disconnect();
  if (retry)
    start();
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

/** Called when Tor has successfully established a circuit. */
void
MainWindow::circuitEstablished()
{
  updateTorStatus(CircuitEstablished);
  setStartupProgress(ui.progressBar->maximum(),
                     tr("Connected to the Tor network!"));
  startSubprocesses();
}

/** Checks the status of the current version of Tor to see if it's old,
 * unrecommended, or obsolete. */
void
MainWindow::checkTorVersion()
{
  QString status;
  if (_torControl->getInfo("status/version/current", status)) {
    if (!status.compare("old", Qt::CaseInsensitive)
          || !status.compare("unrecommended", Qt::CaseInsensitive)
          || !status.compare("obsolete", Qt::CaseInsensitive)) {
      dangerousTorVersion();
    }
  }
}

/** Called when Tor thinks its version is old or unrecommended, and displays a
 * message notifying the user. */
void
MainWindow::dangerousTorVersion()
{
  static bool alreadyWarned = false;

  if (!alreadyWarned) {
    QString website = "https://www.torproject.org/";
#if QT_VERSION >= 0x040200
    website = QString("<a href=\"%1\">%1</a>").arg(website);
#endif

    VMessageBox::information(this,
      tr("Tor Update Available"),
      p(tr("The currently installed version of Tor is out of date or no longer "
           "recommended. Please visit the Tor website to download the latest "
           "version.")) + p(tr("Tor website: %1").arg(website)),
      VMessageBox::Ok);
    alreadyWarned = true;
  }
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

/** Creates and displays the Configuration dialog with the current page set to
 * <b>page</b>. */
void
MainWindow::showConfigDialog(ConfigDialog::Page page)
{
  _configDialog->showWindow(page);
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

    if (TrayIcon::supportsBalloonMessages())
      _trayIcon.showBalloonMessage(title, message, TrayIcon::Information);
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
    _newIdentityAct->setEnabled(true);
    ui.lblNewIdentity->setEnabled(true);
  }
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

