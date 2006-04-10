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
 ***************************************************************/

/** 
 * \file trayicon_x11.h
 * \version $Id: mainwindow.cpp 604 2006-04-08 20:36:55Z edmanm $
 */

#ifndef _TRAYICON_X11_H
#define _TRAYICON_X11_H

#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QEvent>


class TrayIconImpl : protected QLabel 
{
protected:
  /** Constructor */
  TrayIconImpl(const QString &iconFile, const QString &toolTip);

  /** Show the tray icon image. */
  void show();
  /** Hide the tray icon image. */
  void hide();
  /** Modify the tray icon's image. */
  void setIcon(const QString &iconFile);
  /** Modify the tray icon's tooltip. */
  void setToolTip(const QString &toolTip);

  /** Process events when the mouse enters the icon area. */
  void enterEvent(QEvent *event);
};

#endif

