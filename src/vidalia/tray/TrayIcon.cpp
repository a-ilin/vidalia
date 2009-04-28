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
** \file TrayIcon.cpp
** \version $Id$
** \brief Places an icon with context menu in the system notification area
*/

#include "TrayIcon.h"

#include <QSysInfo>


#if defined(Q_WS_MAC)
/* Exported by Qt, but not declared in the header files. 
 * See http://doc.trolltech.com/exportedfunctions.html  */
void qt_mac_set_dock_menu(QMenu *menu);
#endif


/** Default constructor. */
TrayIcon::TrayIcon(QWidget *parent)
  : TrayIconImpl(parent)
{
}

/** Catches and handles mouse-related events. */
bool
TrayIcon::event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::MouseButtonDblClick:
      mouseButtonDblClick((QMouseEvent *)event);
      break;

    default:
      return TrayIconImpl::event(event);
  }
  event->accept();
  return true;
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
#else
  TrayIconImpl::setContextMenu(menu);
#endif
}

/** Displays a balloon message next to the tray icon. */
void
TrayIcon::showBalloonMessage(const QString &title, const QString &message,
                             BalloonMessageIcon balloonIcon)
{
#if defined(Q_WS_MAC)
  Q_UNUSED(title)
  Q_UNUSED(message)
  Q_UNUSED(balloonIcon)
#else
  QSystemTrayIcon::MessageIcon icon;
  switch (balloonIcon) {
    case NoIcon:   icon = QSystemTrayIcon::NoIcon; break;
    case Warning:  icon = QSystemTrayIcon::Warning; break;
    case Critical: icon = QSystemTrayIcon::Critical; break;
    default:  icon = QSystemTrayIcon::Information; break;
  }
  TrayIconImpl::showMessage(title, message, icon);
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
#else
  /* Ask Qt if there is a tray available */
  return QSystemTrayIcon::isSystemTrayAvailable();
#endif
}

/** Returns true if the current platform and tray icon implementation supports
 * tray icon balloon messages. */
bool
TrayIcon::supportsBalloonMessages()
{
#if defined(Q_WS_WIN)
  return (QSystemTrayIcon::supportsMessages()
            && QSysInfo::WindowsVersion > QSysInfo::WV_2000);
#elif defined(Q_WS_MAC)
  return false;
#else
  return QSystemTrayIcon::supportsMessages();
#endif
}

