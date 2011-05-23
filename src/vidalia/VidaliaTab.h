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
** \file VidaliaTab.h
** \brief Main tab that shows the current Tor status
*/

#ifndef _VIDALIATAB_H
#define _VIDALIATAB_H

#include "VSettings.h"

#include <QtGui>

class VidaliaTab : public QWidget
{
  Q_OBJECT

public:
  /** Default constructor. */
  VidaliaTab(const QString &title, const QString &name = "", QWidget *parent = 0);
  /** Destructor. */
  ~VidaliaTab();

  /** Returns the tab's title */
  QString getTitle() const { return _title; }
  /** Sets the tab's title to title */
  void setTitle(const QString &title) { _title = title; }

  /** Associates a shortcut key sequence with a slot. */
  void setShortcut(const QString &shortcut, const char *slot);

  /** Gets the saved value of a property associated with this window object.
   * If no value was saved, the default value is returned. */
  QVariant getSetting(QString name, QVariant defaultValue);
  /** Saves a value associated with a setting name for this window object. */
  void saveSetting(QString name, QVariant value);
  /** Sets the onTop property of the tab to top */
  void setOnTop(bool top);

signals:
  /** Emitted when a VidaliaWindow requests help information on the specified
   * <b>topic</b>. */
  void helpRequested(const QString &topic);

  void closeTab();

protected:
  /** Reimplement the windows' changeEvent() method to check if the event
   * is a QEvent::LanguageChange event. If so, call retranslateUi(), which
   * subclasses of VidaliaWindow can reimplement to update their UI. */
  virtual void changeEvent(QEvent *e);
  /** Called when the user wants to change the currently visible language. */
  virtual void retranslateUi();

  virtual void closeEvent(QCloseEvent *event);

//  virtual void mouseMoveEvent(QMouseEvent *event);

  bool _onTop; /**< True if the current tab is the one being displayed */

private:
  QString _title; /**< Title displayed in the tab */
  VSettings* _settings; /**< Object used to store window properties */
};

#endif

