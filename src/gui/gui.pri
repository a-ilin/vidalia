#################################################################
#  $Id$
#
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006,  Matt Edman, Justin Hipple
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
#  02110-1301, USA.
#################################################################

HEADERS += $$PWD/mainwindow.h
SOURCES += $$PWD/mainwindow.cpp 

RESOURCES += $$PWD/res/vidalia_common.qrc


win32 {
  RC_FILE = $$PWD/res/vidalia_win.rc
}
unix:!macx {
  RESOURCES += $$PWD/res/vidalia_x11.qrc
}
macx {
  # Set the application's icon file
  ICON = $$PWD/res/icons/vidalia.icns
  
  # Add in additional applications icon files
  icons.files = $$PWD/res/icons/tor-on.icns \
                $$PWD/res/icons/tor-off.icns\
                $$PWD/res/icons/tor-starting.icns\
                $$PWD/res/icons/tor-stopping.icns
  icons.path = Contents/Resources
  QMAKE_BUNDLE_DATA += icons
}


include($$PWD/tray/tray.pri)
include($$PWD/about/about.pri)
include($$PWD/log/log.pri)
include($$PWD/bwgraph/bwgraph.pri)
include($$PWD/config/config.pri)
include($$PWD/help/help.pri)
include($$PWD/network/network.pri)

