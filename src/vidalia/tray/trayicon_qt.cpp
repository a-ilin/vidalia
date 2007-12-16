/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
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
 ***************************************************************/

/** 
 * \file trayicon_qt.cpp
 * \version $Id$
 * \brief Tray icon wrapper around Qt's QSystemTrayIcon implementation
 */

#include <QMouseEvent>
#include "trayicon_qt.h"


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

