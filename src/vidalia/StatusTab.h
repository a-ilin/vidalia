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
** \file StatusTab.h
** \brief Main tab that shows the current Tor status
*/

#ifndef _STATUSTAB_H
#define _STATUSTAB_H

#include "ui_StatusTab.h"
#include "VidaliaTab.h"

class StatusTab : public VidaliaTab
{
  Q_OBJECT

public:
  /** Default constructor */
  StatusTab(QWidget *parent = 0);
  /** Destructor. */
  ~StatusTab();

  /** Sets the icon to the general Tor status display */
  void setTorStatus(const QPixmap &icon);
  /** Sets the text to the general Tor status display */
  void setTorStatus(const QString &text);
  /** Wrapper for the first two methods for ease of use */
  void setTorStatus(const QPixmap &icon, const QString &text);
  /** Sets the Tor status display to visible or hidden */
  void setTorStatusVisible(bool visible);

  /** Sets the progress bar and its description */
  void setProgress(int progress, const QString &description);
  /** Sets the visibility of the progress bar */
  void setProgressVisible(bool visible);

  /** Returns true if the "Show on start up" checkbox is checked */
  bool isCheckedShowOnStartup();
  /** Sets "Show on start up" checkbox checked if check is true */
  void checkShowOnStartup(bool check);
  /** Hides the "Show on start up" checkbox */
  void hideCheckShowOnStartup();

private slots:
  /** Called when the "show on startup" checkbox is toggled. */
  void toggleShowOnStartup(bool checked);

protected:
  /** Called when the user changes the UI translation. */
  void retranslateUi();

private:
  Ui::StatusTab ui;
};

#endif
