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
** \file ServerPage.h
** \brief Tor server configuration options
*/

#ifndef _SERVERPAGE_H
#define _SERVERPAGE_H

#include "ui_ServerPage.h"
#include "ConfigPage.h"
#include "TorControl.h"
#include "ServerSettings.h"
#include "ExitPolicy.h"
#include "HelpBrowser.h"

#include <QMessageBox>
#include <QTimer>


class ServerPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  ServerPage(QWidget *parent = 0);
  /** Default Destructor */
  ~ServerPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

  /** Applies the server configuration settings to Tor. Returns true if the
   * settings were applied successfully. Otherwise, <b>errmsg</b> is set and
   * false is returned. */
  bool apply(QString &errmsg);
  /** Reverts the server configuration settings to their values at the last
   * time they were successfully applied to Tor. */
  void revert();
  /** Returns true if the user has changed their server settings since the
   * last time they were applied to Tor. */
  bool changedSinceLastApply();
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private slots:
  /** Called when the user toggles any one of the server mode radio buttons
   * and hides or displays the server configuration tabs appropriately. */
  void serverModeChanged(bool enabled);
  /** Called when the user clicks the bandwidth help button */
  void bandwidthHelp();
  /** Called when the user clicks the exit policy help button */
  void exitPolicyHelp();
  /** Called when the user selects a new value from the rate combo box */
  void rateChanged(int rate);
  /** Called when the user edits the max or average bandwidth limits. */
  void customRateChanged();
  /** Called when Vidalia has authenticated to Tor. If the user's Tor is not
   * recent enough, this disables the bridge server option and displays a
   * warning if the user had previously configured Tor as a bridge. */
  void onAuthenticated();
  /** Called when Vidalia disconnects from Tor. This method reenables the
   * bridge server option. */
  void onDisconnected();
  /** Copies the user's bridge relay identity to the clipboard. */
  void copyBridgeIdentity();
  /** Tests automatic port forwarding with UPnP. */
  void testUpnp();
  /** Called when the user clicks the UPnP test dialog's help button. */
  void upnpHelp();
  /** Called when the user clicks on a QLabel containing a hyperlink. */
  void linkActivated(const QString &url);

  /** Called when the user toggles the Enable Accounting checkbox */
  void toggleAccounting(int state);
  /** Called when the user selects a new item from cmbTime */
  void toggleDisplayDay(const QString &str);

private:
  /** Index values of rate values in the bandwidth limits dropdown box. */
  enum BwRateIndex {
    CableDsl256 = 0, /**< Cable/DSL 256 Kbps */
    CableDsl512,     /**< Cable/DSL 512 Kbps */
    CableDsl768,     /**< Cable/DSL 768 Kbps */
    T1CableDsl1500,  /**< T1/Cable/DSL 1.5 Mbps */
    GreaterThan1500, /**< > 1.5 Mbps */
    CustomBwLimits   /**< Custom bandwidth limits */
  };

  /** Returns the index of the selected item in lstExitPolicies */
  int selectedIndex();

  /** Saves the server's bandwidth average and burst limits. */
  void saveBandwidthLimits();
  /** Loads the server's bandwidth average and burst limits. */
  void loadBandwidthLimits();
  /** Saves the server's exit policies. */
  void saveExitPolicies();
  /** Loads the server's exit policies. */
  void loadExitPolicies();
  /** Loads the user's bridge relay identity into the appropriate widgets. If
   *  the user's bridge is not running, then "Not Running" will be
   *  displayed. Otherwise, either the bridge's "address:port", "fingerprint",
   *  or "address:port fingerprint" will be displayed, depending on whether
   *  our GETCONF and GETINFO commands are successful. */
  void loadBridgeIdentity();
  /** Retrieves bridge usage history from Tor, parses and validates it, and
   * then displays it in a new dialog. */
  void displayBridgeUsage();

  /** A ServerSettings object used to get and set information about how a
   * local Tor server is configured. */
  ServerSettings*  _settings;

  /** Qt Designer generated object */
  Ui::ServerPage ui;

  /** Used to store the dirport value and if the user wants to mirror the
   * directory so that they can be emptied when selecting being a bridge and
   * re-added when selecting relay */
  QString _tmpDirPort;
  bool _tmpMirror;
};

#endif

