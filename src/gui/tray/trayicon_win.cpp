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
 
  /* Initialize the structure representing our tray icon. */
  _nfd.cbSize = sizeof(NOTIFYICONDATA);
  _nfd.hWnd = winId();
  _nfd.uID = TRAY_ICON_ID;
  _nfd.uCallbackMessage = WM_NOTIFYICON;
  _nfd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

  /* We want to know when Explorer crashes and recreates the taskbar. */
  WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));

  /* Add the tool tip to the structure */
  setIcon(iconFile);

  /* And give it an icon */
  setToolTip(toolTip);
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
    Shell_NotifyIcon(NIM_ADD, &_nfd);
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

/** Create an icon for the tray image from a pixmap. */
HICON
TrayIconImpl::createIcon(const QPixmap &pixmap)
{
  HBITMAP hbm = pixmap.toWinHBITMAP(QPixmap::PremultipliedAlpha);

  ICONINFO iconInfo;
  iconInfo.fIcon    = TRUE;
  iconInfo.hbmMask  = hbm;
  iconInfo.hbmColor = hbm;

  HICON hIcon = CreateIconIndirect(&iconInfo);
  DeleteObject(hbm);

  return hIcon;
}

/** Show the tray icon image. */
void
TrayIconImpl::show()
{
  Shell_NotifyIcon(NIM_ADD, &_nfd);
}

/** Hide the tray icon image. */
void
TrayIconImpl::hide()
{
  Shell_NotifyIcon(NIM_DELETE, &_nfd);
}

/** Set the tray icon's tooltip. */
void
TrayIconImpl::setToolTip(const QString &toolTip)
{
#if defined(UNICODE)
  lstrcpynW(_nfd.szTip, (TCHAR*)toolTip.unicode(), 
            qMin(toolTip.length()+1, 64));
#else
  lstrcpynA(_nfd.szTip, (const char*)toolTip.toLocal8Bit(), 
            qMin(toolTip.length()+1, 64));
#endif
  Shell_NotifyIcon(NIM_MODIFY, &_nfd);
}

/** Set the tray icon's image. */
void
TrayIconImpl::setIcon(const QString &iconFile)
{
  _nfd.hIcon = createIcon(QPixmap(iconFile)); 
  Shell_NotifyIcon(NIM_MODIFY, &_nfd);
}

