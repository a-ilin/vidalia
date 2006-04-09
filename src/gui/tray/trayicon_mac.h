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
 ****************************************************************/

/** 
 * \file trayicon_mac.h
 * \version $Id: mainwindow.cpp 604 2006-04-08 20:36:55Z edmanm $
 */

#ifndef _TRAYICON_MAC_H
#define _TRAYICON_MAC_H

#include <QObject>
#include <QPixmap>
#include <QString>


/** Stub class that does nothing on Mac. Maybe someday this will house the
 * dock icon code. */
class TrayIconImpl : protected QObject
{
  Q_OBJECT
    
public:
    TrayIconImpl(const QPixmap &icon, const QString &toolTip)
    { Q_UNUSED(icon); Q_UNUSED(toolTip); }

    void show() {}
    void hide() {}
    void setIcon(const QPixmap &icon) {}
    void setToolTip(const QString &toolTip) {}
};

#endif

