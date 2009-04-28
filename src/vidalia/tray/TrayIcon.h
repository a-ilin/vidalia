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
** \file TrayIcon.h
** \version $Id$
** \brief Places an icon with context menu in the system notification area
*/

#ifndef _TRAYICON_H
#define _TRAYICON_H

#include "config.h"
#if defined(Q_WS_MAC)
#include "TrayIconImpl_mac.h"
#else
#include "TrayIconImpl_qt.h"
#endif

#include <QObject>
#include <QString>
#include <QMenu>
#include <QMouseEvent>


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
  /** Respond to a mouse button being double-clicked. */
  void mouseButtonDblClick(QMouseEvent *event);
};

#endif

