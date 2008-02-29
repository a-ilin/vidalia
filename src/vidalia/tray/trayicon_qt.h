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
** \file trayicon_qt.h
** \version $Id$
** \brief Tray icon wrapper around Qt's QSystemTrayIcon implementation
*/

#ifndef _TRAYICON_QT_H
#define _TRAYICON_QT_H

#include <QSystemTrayIcon>


class TrayIconImpl : public QSystemTrayIcon
{
  Q_OBJECT

protected:
  /** Default constructor. */
  TrayIconImpl(QObject *parent = 0);

  /** Modify the tray icon's image to <b>iconFile</b>. */
  void setIcon(const QString &iconFile);

private slots:
  /** Called when the user interacts with the tray icon and propagates
   * double-click events to the parent object. */
  void onActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif

