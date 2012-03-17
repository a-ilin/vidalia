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
** \file AdvancedPage.h
** \brief Advanced Tor and Vidalia configuration options
*/

#ifndef _ADVANCEDPAGE_H
#define _ADVANCEDPAGE_H

#include "ui_AdvancedPage.h"
#include "ConfigPage.h"
#include "TorSettings.h"

#include <QFileDialog>


class AdvancedPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  AdvancedPage(QWidget *parent = 0);
  /** Default Destructor */
  ~AdvancedPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

  /** Applies the network configuration settings to Tor. Returns true if the
   * settings were applied successfully. Otherwise, <b>errmsg</b> is set
   * and false is returned. */
  bool apply(QString &errmsg);
  /** Reverts the Tor configuration settings to their values at the last
   * time they were successfully applied to Tor. */
  void revert();
  /** Returns true if the user has changed their advanced Tor settings since
   * the last time they were applied to Tor. */
  bool changedSinceLastApply();
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

signals:
  /** Emitted when the user changes torrc file to restart Tor */
  void restartTor();

private slots:
  /** Called when the user selects a different authentication method from the
   * combo box. */
  void authMethodChanged(int index);
  /** Called when the user clicks "Browse" to choose location of Tor config
   * file */
  void browseTorConfig();
  /** Called when the user clicks "Browse" to choose the location of Tor's
   * data directory. */
  void browseTorDataDirectory();
  /** Called when the user clicks "Browse" to choose the location of Tor's
   * socket path. */
  void browseSocketPath();
  /** Called when the user clicks "Browse" to choose location of Tor config
   * file */
  void browsePanicPath();
  /** Called when the user changes from ControlPort to ControlSocket or
   * the other way aroud */
  void toggleControl(bool);

  /** Called when the user clicks "Edit current torrc" to edit Tor's config in
   * a more advanced way */
  void displayTorrcDialog();

  /** Called when the user checks "Configure ControlPort automatically" */
  void toggleAuto(bool);

  /** Called when the user checks "Randomly Generate" checkbox */
  void displayWarning(bool);

private:
  /** Returns the authentication method for the given <b>index</b>. */
  TorSettings::AuthenticationMethod indexToAuthMethod(int index);
  /** Returns the index in the authentication methods combo box for the given
   * authentication <b>method</b>. */
  int authMethodToIndex(TorSettings::AuthenticationMethod method);

#if 0
#if defined(Q_WS_WIN)
  /** Installs or removes the Tor service as necessary */
  void setupService(bool useService);
#endif
#endif

  /** A TorSettings object used to save/load settings */
  TorSettings* _settings;
  /** Qt Designer generated object */
  Ui::AdvancedPage ui;
};

#endif

