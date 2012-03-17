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
** \file GeneralPage.h
** \brief General Tor and Vidalia configuration options
*/

#ifndef _GENERALPAGE_H
#define _GENERALPAGE_H

#include "ui_GeneralPage.h"
#include "ConfigPage.h"
#include "VidaliaSettings.h"
#include "TorSettings.h"

#include <QFileDialog>


class GeneralPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  GeneralPage(QWidget *parent = 0);
  /** Default Destructor */
  ~GeneralPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

signals:
  /** Emitted when the user clicks the "Check Now" button. */
  void checkForUpdates();

private slots:
  /** Open a QFileDialog to browse for a Tor executable file. */
  void browseTorExecutable();
  /** Open a QFileDialog to browse for a proxy executable file. */
  void browseProxyExecutable();
  /** Initiate an immediate check for software updates. */
  void updateNow();

private:
  /** Displays a file dialog allowing the user to browse for an executable
   * file. <b>caption</b> will be displayed in the dialog's title bar and <b>
   * file</b>, if specified, is the default file selected in the dialog. */
  QString browseExecutable(const QString &caption,
                           const QString &file = QString());

  /** A VidaliaSettings object used for saving/loading vidalia settings */
  VidaliaSettings *_vidaliaSettings;
  /** A TorSettings ovject used for saving/loading tor settings */
  TorSettings *_torSettings;
  /** Qt Designer generated object */
  Ui::GeneralPage ui;
};

#endif

