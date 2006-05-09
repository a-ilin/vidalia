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
 *****************************************************************
 *  The createIcon() method in this class is derived from code in
 *  trayicon_win.cpp by Justin Karneges, licensed as follows:
 *
 *  Copyright (C) 2003  Justin Karneges
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 ****************************************************************/

/** 
 * \file trayicon_win.cpp
 * \version $Id$
 */

#include <QApplication>

#include "trayicon_win.h"

#define WM_NOTIFYICON   WM_APP /**< Message sent for events related to our icon.*/
#define TRAY_ICON_ID    0   /**< ID for our tray icon. */
UINT    WM_TASKBARCREATED;  /**< Message sent when taskbar is created. */


/** Constructor. */
TrayIconImpl::TrayIconImpl(const QString &iconFile, const QString &toolTip)
{
  setObjectName("trayiconimpl");
 
  /* We want to know when Explorer crashes and recreates the taskbar. */
  WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
  
  /* Set the tray icon image */
  setIcon(iconFile);
  
  /* Add the tool tip to the structure */
  setToolTip(toolTip);
}

/** Updates the tray icon. */
void
TrayIconImpl::updateTrayIcon(DWORD msg)
{
QT_WA(  
  { /* UNICODE */
    NOTIFYICONDATAW nfd;
    nfd.cbSize = sizeof(NOTIFYICONDATAW);
    nfd.hWnd = winId();
    nfd.uID = TRAY_ICON_ID;
    nfd.uCallbackMessage = WM_NOTIFYICON;
    nfd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nfd.hIcon = _icon;
    lstrcpynW(nfd.szTip, (TCHAR*)_toolTip.unicode(), 
              qMin(_toolTip.length()+1, 64));
    Shell_NotifyIconW(msg, &nfd);
  },
  { /* non-UNICODE */
    NOTIFYICONDATAA nfd;
    nfd.cbSize = sizeof(NOTIFYICONDATAA);
    nfd.hWnd = winId();
    nfd.uID = TRAY_ICON_ID;
    nfd.uCallbackMessage = WM_NOTIFYICON;
    nfd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nfd.hIcon = _icon;
    lstrcpynA(nfd.szTip, (const char*)_toolTip.toLocal8Bit(), 
              qMin(_toolTip.length()+1, 64));
    Shell_NotifyIconA(msg, &nfd);
  })
}

/** Catches and handles mouse-related native Windows event messages. */
bool
TrayIconImpl::winEvent(MSG *msg, long *result)
{
  if (msg->message == WM_NOTIFYICON) {
    switch (msg->lParam) {
      case WM_LBUTTONUP:
        return sendMouseEvent(QEvent::MouseButtonRelease, Qt::LeftButton);

      case WM_RBUTTONUP:
        return sendMouseEvent(QEvent::MouseButtonRelease, Qt::RightButton);

      case WM_LBUTTONDBLCLK:
        return sendMouseEvent(QEvent::MouseButtonDblClick, Qt::LeftButton);

      default:
        break;
    }
  } else if (msg->message == WM_TASKBARCREATED) {
    /* We handle WM_TASKBARCREATED because it's possible that Explorer
     * crashed and was recreated, in which case we need to re-add our icon. */
    updateTrayIcon(NIM_ADD);
  }
  return QWidget::winEvent(msg, result);
}

/** Sends a mouse-related event to the main TrayIcon class. */
bool
TrayIconImpl::sendMouseEvent(QEvent::Type type, Qt::MouseButton button)
{
  QPoint pos = QCursor::pos();
  QMouseEvent event(type, mapFromGlobal(pos), pos, button, button, Qt::NoModifier);
  return QApplication::sendEvent(this, &event);
}

/** Create an icon for the tray image from a resource identifier. */
HICON
TrayIconImpl::createIcon(const int resourceId)
{
  return LoadIconA((HINSTANCE)GetModuleHandleA(NULL),
                   MAKEINTRESOURCEA(resourceId));
}

/** Show the tray icon image. */
void
TrayIconImpl::show()
{
  updateTrayIcon(NIM_ADD);
}

/** Hide the tray icon image. */
void
TrayIconImpl::hide()
{
  updateTrayIcon(NIM_DELETE);
}

/** Set the tray icon's tooltip. */
void
TrayIconImpl::setToolTip(const QString &toolTip)
{
  _toolTip = toolTip;
  updateTrayIcon(NIM_MODIFY);
}

/** Set the tray icon's image. */
void
TrayIconImpl::setIcon(const QString &iconFile)
{
  _icon = createIcon(iconFile.toInt());
  updateTrayIcon(NIM_MODIFY);
}

