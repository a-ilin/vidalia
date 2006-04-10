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
 * \file trayicon_win.h
 * \version $Id: mainwindow.cpp 604 2006-04-08 20:36:55Z edmanm $
 */

#ifndef _TRAYICON_MAC_H
#define _TRAYICON_MAC_H

#include <Carbon/Carbon.h>

#include <QWidget>
#include <QString>


class TrayIconImpl : protected QWidget
{
protected:
  /** Constructor */
  TrayIconImpl(const QString &iconFile, const QString &toolTip);
  /** Destructor */
  ~TrayIconImpl();

  /** Show the tray icon. */
  void show();
  /** Hide the tray icon. */
  void hide();
  /** Update the tray icon's image. */
  void setIcon(const QString &iconFile);
  /** Update the tray icon's tooltip. */
  void setToolTip(const QString &toolTip);

private:
  /** Load icon data from the given file and create a CGImageRef. */
  CGImageRef createIconFromFile(FSSpec fileSpec);
  /** Create an icon from the given filename in the application bundle. */
  CGImageRef createIcon(const QString &iconFile);
  /** Callback used by CGDataProviderCreateWithData(). */
  static void releaseCallback(void *info, const void *data, size_t size);
  
  CGImageRef _imageRef; /**< Tray icon image. */
  bool _shown; /**< True if the dock icon is to be displayed. */
};

#endif

