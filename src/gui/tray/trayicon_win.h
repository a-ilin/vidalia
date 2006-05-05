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
 * \file trayicon_win.h
 * \version $Id: mainwindow.cpp 604 2006-04-08 20:36:55Z edmanm $
 */

#ifndef _TRAYICON_WIN_H
#define _TRAYICON_WIN_H

#include <windows.h>

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMouseEvent>


class TrayIconImpl : protected QWidget
{
protected:
  /** Constructor */
  TrayIconImpl(const QString &iconFile, const QString &toolTip);

  /** Show the tray icon. */
  void show();
  /** Hide the tray icon. */
  void hide();
  /** Update the tray icon's image. */
  void setIcon(const QString &iconFile);
  /** Update the tray icon's tooltip. */
  void setToolTip(const QString &toolTip);

private:
  /** Catches native Windows event messages. */
  bool winEvent (MSG *message, long *result);
  /** Sends a new Qt-based mouse event. */
  bool sendMouseEvent(QEvent::Type type, Qt::MouseButton button);
  /** Create an icon for the tray image from a given icon resource identifier. */
  HICON createIcon(const int resourceId);
  /** Updates the tray icon. */
  void updateTrayIcon(DWORD msg);

  HICON _icon;      /**< The tray icon image. */
  QString _toolTip; /**< The tray icon's tooltip. */
};

#endif

