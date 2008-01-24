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
 * \file trayicon.h
 * \version $Id$
 * \brief Places an icon with context menu in the system notification area
 */

#ifndef _TRAYICON_H
#define _TRAYICON_H

#include <QObject>
#include <QString>
#include <QMenu>
#include <QMouseEvent>
#include "config.h"

/* Include the correct tray icon implementation */
#if defined(USE_QSYSTEMTRAYICON)
#include "trayicon_qt.h"
#elif defined(Q_WS_WIN)
#include "trayicon_win.h"
#elif defined(Q_WS_X11)
#include "trayicon_x11.h"
#else
#include "trayicon_mac.h"
#endif


class TrayIcon : public TrayIconImpl
{
  Q_OBJECT

public:
  /** Balloon message status icons. */
  enum BalloonMessageIcon {
    NoIcon = 0,
    Information,
    Warning,
    Critical
  };

  /** Default constructor. */
  TrayIcon(QWidget *parent = 0);

  /** Show the tray icon. */
  void show();
  /** Hide the tray icon. */
  void hide();
  /** Updates the icon image and tooltip. */
  void update(const QString &iconFile, const QString &toolTip);
  /** Update the tray icon's tooltip. */
  void setToolTip(const QString &toolTip);
  /** Update the tray icon's image. */
  void setIcon(const QString &iconFile);
  /** Sets the context menu displayed when the tray icon is selected. */
  void setContextMenu(QMenu *contextMenu);
  /** Displays a balloon message next to the tray icon. */
  void showBalloonMessage(const QString &title, const QString &message,
                          BalloonMessageIcon icon);

  /** Returns true if the current platform and tray icon implementation
   * supports tray icons. */
  static bool isTrayIconSupported();
  /** Returns true if the current platform and tray icon implementation
   * supports tray icon balloon messages. */
  static bool supportsBalloonMessages();

signals:
  /** Emitted when the user double-clicks on the tray icon. */
  void doubleClicked();

protected:
  /** Override's QObject' event() method to catch mouse-related events. */
  bool event(QEvent *);
  /** Respond to a mouse button being pressed. */ 
  void mouseButtonPress(QMouseEvent *event);
  /** Respond to a mouse button being released. */
  void mouseButtonRelease(QMouseEvent *event);
  /** Respond to a mouse button being double-clicked. */
  void mouseButtonDblClick(QMouseEvent *event);

private:
  QMenu* _contextMenu; /**< Menu to display when the tray icon is clicked. */
};

#endif

