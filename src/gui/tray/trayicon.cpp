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
 */

#include "trayicon.h"

/** Constructor. */
TrayIcon::TrayIcon(const QString &iconFile, const QString &toolTip, QMenu *popupMenu)
: TrayIconImpl(iconFile, toolTip)
{
  _popupMenu = popupMenu;
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
#if defined(Q_WS_X11)
  if (_popupMenu) {
    _popupMenu->popup(event->globalPos());
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
  if (_popupMenu) {
    /* This little activateWindow() dance is necessary to make menu closing
     * work properly on Windows. */
    _popupMenu->activateWindow();
    _popupMenu->popup(event->globalPos());
    _popupMenu->activateWindow();
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

