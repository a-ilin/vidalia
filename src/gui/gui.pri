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

HEADERS += $$PWD/mainwindow.h \
           
SOURCES += $$PWD/main.cpp \
           $$PWD/mainwindow.cpp 

RESOURCES += $$PWD/res/vidalia.qrc

macx {
  # Set the application's icon file
  ICON = $$PWD/res/vidalia.icns
  
  # Add in additional applications icon files
  icons.files = $$PWD/res/tor_on.icns \
                $$PWD/res/tor_off.icns
  icons.path = Contents/Resources
  QMAKE_BUNDLE_DATA += icons
  
  include($$PWD/dock/dock.pri)
}
win32 {
  RC_FILE = $$PWD/res/vidalia.rc
}

include($$PWD/tray/tray.pri)
include($$PWD/aboutdialog/aboutdialog.pri)
include($$PWD/messagelog/messagelog.pri)
include($$PWD/bwgraph/bwgraph.pri)
