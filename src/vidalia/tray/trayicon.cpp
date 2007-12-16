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
 * \file trayicon.cpp
 * \version $Id$
 * \brief Places an icon with context menu in the system notification area
 */

#include <QSysInfo>
#include "trayicon.h"

#if defined(Q_WS_MAC)
/* Exported by Qt, but not declared in the header files. 
 * See http://doc.trolltech.com/exportedfunctions.html  */
void qt_mac_set_dock_menu(QMenu *menu);
#endif


/** Default constructor. */
TrayIcon::TrayIcon(QWidget *parent)
  : TrayIconImpl(parent)
{
  _contextMenu = 0;
}

/** Catches and handles mouse-related events. */
bool
TrayIcon::event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::MouseButtonPress:
      mouseButtonPress((QMouseEvent *)event);
      break;

    case QEvent::MouseButtonRelease:
      mouseButtonRelease((QMouseEvent *)event);
      break;

    case QEvent::MouseButtonDblClick:
      mouseButtonDblClick((QMouseEvent *)event);
      break;

    default:
      return TrayIconImpl::event(event);
  }
  event->accept();
  return true;
}

/** Responds to a single mouse button press. On X11, we display the menu when
 * a mouse button is pressed. */
void
TrayIcon::mouseButtonPress(QMouseEvent *event)
{
#if defined(Q_WS_X11) && !defined(HAVE_QSYSTEMTRAYICON_H)
  if (_contextMenu) {
    _contextMenu->popup(event->globalPos());
  }
#else
  Q_UNUSED(event);
#endif
}

/** Responds to a single mouse button release. On Windows, we display the menu
 * when a mouse button is released. */
void
TrayIcon::mouseButtonRelease(QMouseEvent *event)
{
#if defined(Q_WS_WIN)
  if (_contextMenu) {
    /* This little activateWindow() dance is necessary to make menu closing
     * work properly on Windows. */
    _contextMenu->activateWindow();
    _contextMenu->popup(event->globalPos());
    _contextMenu->activateWindow();
  }
#else
  Q_UNUSED(event);
#endif
}

/** Responds to a mouse button double-click. On all platforms, we just emit a
 * signal and let the owner of the tray icon decide if they want to do
 * anything. */
void
TrayIcon::mouseButtonDblClick(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
}

/** Update the tray icon's image and tooltip. */
void
TrayIcon::update(const QString &iconFile, const QString &toolTip)
{
  setIcon(iconFile);
  setToolTip(toolTip);
}

/** Call the platform's tray icon implementation to show the tray icon. */
void
TrayIcon::show()
{
  TrayIconImpl::show();
}

/** Call the platform's tray icon implementation to hide the tray icon. */
void
TrayIcon::hide()
{
  TrayIconImpl::hide();
}

/** Call the platform's tray icon implementation to update the icon's tooltip.*/
void
TrayIcon::setToolTip(const QString &toolTip)
{
  TrayIconImpl::setToolTip(toolTip);
}

/** Call the platform's tray icon implementation to update the icon image. */
void
TrayIcon::setIcon(const QString &iconFile)
{
  TrayIconImpl::setIcon(iconFile);
}

/** Sets the context menu displayed when the tray icon is selected. On Mac,
 * the context menu is displayed when the dock icon is clicked. */
void
TrayIcon::setContextMenu(QMenu *menu)
{
#if defined(Q_WS_MAC)
  qt_mac_set_dock_menu(menu);
#elif defined(HAVE_QSYSTEMTRAYICON_H)
  TrayIconImpl::setContextMenu(menu);
#else
  _contextMenu = menu;
#endif
}

/** Displays a balloon message next to the tray icon. */
void
TrayIcon::showBalloonMessage(const QString &title, const QString &message,
                             BalloonMessageIcon balloonIcon)
{
#if defined(HAVE_QSYSTEMTRAYICON_H)
  QSystemTrayIcon::MessageIcon icon;
  switch (balloonIcon) {
    NoIcon:   icon = QSystemTrayIcon::NoIcon; break;
    Warning:  icon = QSystemTrayIcon::Warning; break;
    Critical: icon = QSystemTrayIcon::Critical; break;
    default:  icon = QSystemTrayIcon::Information; break;
  }
  TrayIconImpl::showMessage(title, message, icon);
#else
  Q_UNUSED(title)
  Q_UNUSED(message)
  Q_UNUSED(balloonIcon)
#endif
}

/** Returns true if the current platform and tray icon implementation supports
 * tray icons. */
bool
TrayIcon::isTrayIconSupported()
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
  /* We always have a tray on Win32 or a dock on OS X */
  return true;
#elif defined(HAVE_QSYSTEMTRAYICON_H)
  /* Ask Qt if there is a tray available */
  return QSystemTrayIcon::isSystemTrayAvailable();
#else
  /* XXX:This is too optimistic, but we need to make our own tray icon
   * implementation smart enough to detect a system tray on X11. */
  return true;
#endif
}

/** Returns true if the current platform and tray icon implementation supports
 * tray icon balloon messages. */
bool
TrayIcon::supportsBalloonMessages()
{
#if defined(HAVE_QSYSTEMTRAYICON_H)
#if defined(Q_WS_WIN)
  return (QSystemTrayIcon::supportsMessages()
            && QSysInfo::WindowsVersion > QSysInfo::WV_2000);
#else
  return QSystemTrayIcon::supportsMessages();
#endif
#else
  return false;    
#endif
}

