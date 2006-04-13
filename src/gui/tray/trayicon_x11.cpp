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
 ***************************************************************
 * This code is derived from systemtray.cpp from QMPDClient. It is 
 * licensed as follows:
 *
 * QMPDClient - An MPD client written in Qt 4.
 * Copyright (C) 2005 Håvard Tautra Knutsen <havtknut@tihlde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "trayicon_x11.h"

#include <QApplication>
#include <QTimer>
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

// System Tray Protocol Specification opcodes.
#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2


TrayIconImpl::TrayIconImpl(const QString &iconFile, const QString &toolTip)
{
  setObjectName("trayiconimpl");
  setMinimumSize(22, 22);
  setMaximumSize(22, 22);
  //setBackgroundRole(QPalette::Base);
  //setBackgroundRole(QPalette::NoRole);

  Display *dpy = QX11Info::display();
  Screen *screen = XDefaultScreenOfDisplay(dpy);
  WId trayWin  = winId();

  /* Standard tray window protocol */
  int iScreen = XScreenNumberOfScreen(screen);
  char szAtom[32];
  snprintf(szAtom, sizeof(szAtom), "_NET_SYSTEM_TRAY_S%d", iScreen);
  Atom selectionAtom = XInternAtom(dpy, szAtom, false);
  XGrabServer(dpy);
  Window win = XGetSelectionOwner(dpy, selectionAtom);
  if (win != None)
      XSelectInput(dpy, win, StructureNotifyMask);
  XUngrabServer(dpy);
  XFlush(dpy);

  if (win != None) {
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.xclient.type = ClientMessage;
    ev.xclient.window = win;
    ev.xclient.message_type = XInternAtom (dpy, "_NET_SYSTEM_TRAY_OPCODE", false);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = CurrentTime;
    ev.xclient.data.l[1] = SYSTEM_TRAY_REQUEST_DOCK;
    ev.xclient.data.l[2] = trayWin;
    ev.xclient.data.l[3] = 0;
    ev.xclient.data.l[4] = 0;
    XSendEvent(dpy, win, false, NoEventMask, &ev);
    XSync(dpy, false);
  }

  /* KDE-related */
  long data = 0;
  Atom dockwindow = XInternAtom(dpy, "KWM_DOCKWINDOW", false);
  Atom traywindow = XInternAtom(dpy, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
  XChangeProperty(dpy, winId(), dockwindow, dockwindow, 32, PropModeReplace, (uchar*)&data, 1);
  XChangeProperty(dpy, winId(), traywindow, XA_WINDOW, 32, PropModeReplace, (uchar*)&data, 1);
 
  /* Set the initial icon and tooltip */
  setIcon(iconFile);
  setToolTip(toolTip);

  //setAttribute(Qt::WA_NoBackground);
  //XSetWindowBackgroundPixmap(dpy, trayWin, None);
}

/** Process events when the mouse enters the icon area. */
void 
TrayIconImpl::enterEvent(QEvent *event) 
{
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.xfocus.display = QX11Info::display();
    ev.xfocus.type = FocusIn;
    ev.xfocus.window = winId();
    ev.xfocus.mode = NotifyNormal;
    ev.xfocus.detail = NotifyAncestor;
    qApp->x11ProcessEvent(&ev);
    QWidget::enterEvent(event);
}

/** Show the tray icon image. */
void
TrayIconImpl::show()
{
  static bool shown = false;
  if (!shown) {
    /* Sometimes the tray icon on my gnome desktop wouldn't dock and instead
     * would appear as its own window. Looking at comments in other projects
     * that used tray icons told me that this is a known problem and that gnome
     * needs a delay before adding the icon the first time. The hack is to give
     * gnome this delay so the icon appears properly docked. */
    QTimer::singleShot(250, this, SLOT(show()));
    shown = true;
  } else {
    QLabel::show();
  }
}

/** Hide the tray icon image. */
void
TrayIconImpl::hide()
{
  QLabel::hide();
}

/** Set the tray icon's tooltip. */
void
TrayIconImpl::setToolTip(const QString &toolTip)
{
  QLabel::setToolTip(toolTip);
}

/** Set the tray icon's image. */
void
TrayIconImpl::setIcon(const QString &iconFile)
{
  /* Load the pixmap image from the specified resource file */
  QPixmap icon(iconFile);
  
  /* Scale the image to the correct size. */
  QPixmap scaledPixmap = icon.scaled(QWidget::size(), 
                                     Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);
  setPixmap(scaledPixmap);
}

