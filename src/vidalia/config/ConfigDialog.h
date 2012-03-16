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
** \file ConfigDialog.h
** \brief Contains a series of Vidalia and Tor configuration pages
*/

#ifndef _CONFIGDIALOG_H
#define _CONFIGDIALOG_H

#include "ui_ConfigDialog.h"
#include "VidaliaWindow.h"

#include <QMainWindow>
#include <QFileDialog>


class ConfigDialog : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Config dialog pages. */
  enum Page {
    General = 0,  /** General configuration page. */
    Network,      /** Network configuration page. */
    Server,       /** Server configuration page. */
    Service,      /** Service Configuration page */
    Appearance,   /** Appearance configuration page. */
    Advanced      /** Advanced configuration page. */
  };

  /** Default Constructor */
  ConfigDialog(QWidget *parent = 0);

public slots:
  /** Shows the config dialog with focus set to the given page. */
  void showWindow(Page page = General);

signals:
  /** Emitted when the user clicks "Check Now" to initiate a check
   * for software updates. */
  void checkForUpdates();
  /** Emitted when the user changes torrc file to restart Tor */
  void restartTor();

protected:
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private slots:
  /** Called when user clicks "Save Settings". Saves their settings to
   * Vidalia's configuration file. */
  void saveChanges();
  /** Called after Vidalia has authenticated to Tor and applies any changes
   * made since the last time they were applied. */
  void applyChanges();
  /** Sends Tor a SAVECONF to write its configuration to disk. If the
   * SAVECONF is successful, then all settings are considered to be
   * applied. */
  void saveConf();
  /** Called when a ConfigPage in the dialog requests help on a specific
   * <b>topic</b>. */
  void help(const QString &topic);
  /** Shows general help information for whichever settings page the user is
   * currently viewing. */
  void help();
  /** Stub method that relays the checkForUpdates() signal from the General
   * settings page to the owner of the config dialog (MainWindow). */
  void onCheckForUpdates();
  /** Loads the current configuration settings */
  void loadSettings();

private:
  /** Creates a new action for a config page. */
  QAction* createPageAction(const QIcon &img, const QString &text,
                            const QString &data, QActionGroup *group);
  /** Adds a new action to the toolbar. */
  void addAction(QAction *action, const char *slot = 0);

  /** Qt Designer generated object */
  Ui::ConfigDialog ui;
};

#endif

