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
 * \file trayicon_qt.h
 * \version $Id$
 * \brief Tray icon wrapper around Qt's QSystemTrayIcon implementation
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

