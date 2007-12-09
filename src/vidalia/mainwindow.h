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
 * \file mainwindow.h
 * \version $Id$
 * \brief Main (hidden) window. Creates tray menu and child windows
 */

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <torcontrol.h>

/* QSystemTrayIcon appeared in Qt 4.2, but we need a bugfix to it on Mac 
 * that won't appear until Qt 4.2.2. */
#if QT_VERSION >= 0x040200 && !defined(Q_WS_MAC)
#define USE_QSYSTEMTRAYICON  1
#include <QSystemTrayIcon>
#else
#undef USE_QSYSTEMTRAYICON
#include "tray/trayicon.h"
#endif

#include "vidaliawindow.h"
#include "about/aboutdialog.h"
#include "log/messagelog.h"
#include "bwgraph/bwgraph.h"
#include "config/configdialog.h"
#include "help/browser/helpbrowser.h"
#include "network/netviewer.h"
#include "ui_mainwindow.h"
#include "browserprocess.h"


class MainWindow : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Default constructor */
  MainWindow();
  /** Destructor. */
  ~MainWindow();

protected:
  /** Catches and processes Tor client status events. */
  virtual void customEvent(QEvent *event);

private slots:
  /** Displays the help browser and displays the most recently viewed help
   * topic. */
  void showHelpDialog();
  /** Called when a child window requests the given help <b>topic</b>. */
  void showHelpDialog(const QString &topic);
  /** Called when the user selects "Start" from the menu. */
  void start();
  /** Called when the Tor process fails to start. */
  void startFailed(QString errmsg);
  /** Called when the Tor process has successfully started. */
  void started();
  /** Called when the user selects "Stop" form the menu. */
  bool stop();
  /** Called when the Tor process has exited, either expectedly or not. */
  void stopped(int errorCode, QProcess::ExitStatus exitStatus);
  /** Called when the control socket has connected to Tor. */
  void connected();
  /** Called when the control connection fails. */
  void connectFailed(QString errmsg);
  /** Called when Vidalia wants to disconnect from a Tor it did not start. */
  void disconnect();
  /** Called when the control socket has been disconnected. */
  void disconnected();
  /** Called when Vidalia has successfully authenticated to Tor. */
  void authenticated();
  /** Called when Vidalia fails to authenticate to Tor. The failure reason is
   * specified in <b>errmsg</b>. */
  void authenticationFailed(QString errmsg);
  /** Re-enables the 'New Identity' button after a delay from the previous time
   * 'New Identity' was used. */
  void enableNewIdentity();
  /** Called when the user selects the "New Identity" action from the menu. */
  void newIdentity();
  /** Called when the user exits Vidalia. */
  void close();
  /** Called when the application has started and the main event loop is
   * running. */
  void running();
  /** Terminate the Tor process if it is being run under Vidalia, disconnect
   * all TorControl signals, and exit Vidalia. */
  void shutdown();
  /** Creates and displays Vidalia's About dialog. */
  void showAboutDialog();
  /** Creates and displays the Configuration dialog with the current page set
   * to <b>page</b>. */
  void showConfigDialog(ConfigDialog::Page page = ConfigDialog::General);
  /** Displays the Configuration dialog, set to the Server page. */
  void showServerConfigDialog();
  /** Called when the "show on startup" checkbox is toggled. */
  void toggleShowOnStartup(bool checked);
  /** Called when the web browser has stopped */
  void onBrowserFinished(int exitCode, QProcess::ExitStatus exitStatus);
  /** Called web the web browser failed to start */
  void onBrowserFailed(QString errmsg);
  
#if QT_VERSION >= 0x040200 && !defined(Q_WS_MAC)
  /** Displays the main window if <b>reason</b> is DoubleClick. */
  void trayActivated(QSystemTrayIcon::ActivationReason reason);
#endif

private:
  enum TorStatus {
    Unset,      /**< Tor's status has not yet been set. */
    Stopping,   /**< Tor is in the process of shutting down. */
    Stopped,    /**< Tor is not running. */
    Starting,   /**< Tor is in the process of starting. */
    Started,    /**< Tor is currently running. */
    Authenticating, /**< Vidalia is authenticating to Tor. */
    Authenticated,  /**< Vidalia has authenticated to Tor. */
    CircuitEstablished /**< Tor has built a circuit. */
  };
  /** Create the actions on the tray menu or menubar */
  void createActions();
  /** Creates a tray icon with a context menu and adds it to the system
   * notification area. On Mac, we also set up an application menubar. */
  void createTrayIcon();
  /** Create the tray popup menu and it's submenus */
  QMenu* createTrayMenu();
  /** Creates a default menubar on Mac */
  void createMenuBar();
  /** Returns true if we're running on a platform with tray icon support. */
  bool isTrayIconSupported();
  /** Updates the UI to reflect Tor's current <b>status</b>. Returns the
   * previously set TorStatus value. */
  TorStatus updateTorStatus(TorStatus status);
  /** Starts the web browser, if appropriately configured */
  void startBrowser();
  /** Converts a TorStatus enum value to a string for debug logging purposes. */
  QString toString(TorStatus status);
  /** Authenticates Vidalia to Tor's control port. */
  bool authenticate();
  /** Searches for and attempts to load the control authentication cookie.
   * This assumes the cookie is named 'control_auth_cookie'. If
   * <b>cookiePath</b> is empty, this method will search some default locations
   * depending on the current platform. <b>cookiePath</b> can point to either
   * a cookie file or a directory containing the cookie file. */
  QByteArray loadControlCookie(QString cookiePath = QString());
  /** Called when Tor has successfully established a circuit. */
  void circuitEstablished();
  /** Checks the status of the current version of Tor to see if it's old,
   * unrecommended, or obsolete. */
  void checkTorVersion();
  /** Called when Tor thinks its version is old or unrecommended, and displays
   * a message notifying the user. */
  void dangerousTorVersion();

  /** The current status of Tor. */
  TorStatus _status;
  /** Used to determine if the Tor process exiting was intentional or not */
  bool _isIntentionalExit;
  /** Tracks whether we started a delayed server shutdown. */
  bool _delayedShutdownStarted;
  /** Set to true if Vidalia started its own Tor process. */
  bool _isVidaliaRunningTor;
  /** A MessageLog object which handles logging Tor messages */
  MessageLog* _messageLog;
  /** A BandwidthGraph object which handles monitoring Tor bandwidth usage */
  BandwidthGraph* _bandwidthGraph;
  /** A NetViewer object which displays the Tor network graphically */
  NetViewer* _netViewer;
  /** A ConfigDialog object which lets the user configure Tor and Vidalia */
  ConfigDialog* _configDialog;
  /** A TorControl object that handles communication with Tor */
  TorControl* _torControl;
  /** A BrowserProcess object that manages the web browser */
  BrowserProcess* _browserProcess;
  /** Remembers the control password between when we start Tor with a hash of
   * the password and when we need to provide the password itself. */
  QString _controlPassword;

#if QT_VERSION >= 0x040200 && !defined(Q_WS_MAC)
  QSystemTrayIcon _trayIcon; /**< The Vidalia icon that sits in the tray.
                                  (post-Qt 4.2) */
#else
  TrayIcon _trayIcon; /**< The Vidalia icon that sits in the tray. (pre-Qt 4.2) */
#endif
 
  /** Defines the actions for the tray menu */
  QAction* _controlPanelAct;
  QAction* _startStopAct;
  QAction* _configAct;
  QAction* _aboutAct;
  QAction* _exitAct;
  QAction* _bandwidthAct;
  QAction* _messageAct;
  QAction* _helpAct;
  QAction* _networkAct;
  QAction* _newIdentityAct;

  Ui::MainWindow ui; /**< Qt Designer generated object. */
};

#endif

