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
** \file TrayIconImpl_qt.cpp
** \version $Id$
** \brief Tray icon wrapper around Qt's QSystemTrayIcon implementation
*/

#include "TrayIconImpl_qt.h"

#include <QMouseEvent>


/** Default constructor. */
TrayIconImpl::TrayIconImpl(QObject *parent)
  : QSystemTrayIcon(parent)
{
  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
}

/** Modify the tray icon's image to <b>iconFile</b>. */
void
TrayIconImpl::setIcon(const QString &iconFile)
{
  QSystemTrayIcon::setIcon(QIcon(iconFile));
}

/** Called when the user interacts with the tray icon and propoagates
 * double-click events to the parent object. */
void
TrayIconImpl::onActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::DoubleClick) {
    /* Propagate the double-click to the parent class's event handler */
    event(new QMouseEvent(QEvent::MouseButtonDblClick, QPoint(), 
                          Qt::LeftButton, Qt::LeftButton, 
                          Qt::NoModifier));
  }
}

