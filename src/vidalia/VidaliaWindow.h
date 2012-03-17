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
** \file VidaliaWindow.h
** \brief Common superclass for all Vidalia windows
*/

#ifndef _VIDALIAWINDOW_H
#define _VIDALIAWINDOW_H

#include "VSettings.h"

#include <QString>
#include <QWidget>
#include <QVariant>
#include <QMainWindow>


class VidaliaWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor. */
  VidaliaWindow(const QString &name, QWidget *parent = 0,
                Qt::WFlags flags = 0);
  /** Destructor. */
  ~VidaliaWindow();

  /** Associates a shortcut key sequence with a slot. */
  void setShortcut(const QString &shortcut, const char *slot);
  /** Saves the size and location of the window. */
  void saveWindowState();
  /** Restores the last size and location of the window. */
  void restoreWindowState();

  /** Gets the saved value of a property associated with this window object.
   * If no value was saved, the default value is returned. */
  QVariant getSetting(QString name, QVariant defaultValue);
  /** Saves a value associated with a setting name for this window object. */
  void saveSetting(QString name, QVariant value);

protected:
  /** Reimplement the windows' changeEvent() method to check if the event
   * is a QEvent::LanguageChange event. If so, call retranslateUi(), which
   * subclasses of VidaliaWindow can reimplement to update their UI. */
  virtual void changeEvent(QEvent *e);
  /** Called when the user wants to change the currently visible language. */
  virtual void retranslateUi();

public slots:
  /** Shows or hides this window. */
  virtual void setVisible(bool visible);
  /** Show this window. This method really just exists for subclasses to
   * override, since QMainWindow::show() is non-virtual. */
  virtual void showWindow() { QMainWindow::show(); }

signals:
  /** Emitted when a VidaliaWindow requests help information on the specified
   * <b>topic</b>. */
  void helpRequested(const QString &topic);

private:
  QString _name;  /**< Name associated with this window. */
  VSettings* _settings; /**< Object used to store window properties */
};

#endif

