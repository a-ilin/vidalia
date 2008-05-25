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
** \file advancedpage.h
** \version $Id$
** \brief Advanced Tor and Vidalia configuration options
*/

#ifndef _ADVANCEDPAGE_H
#define _ADVANCEDPAGE_H

#include <QFileDialog>
#include <config/torsettings.h>

#include "configpage.h"
#include "ui_advancedpage.h"


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

