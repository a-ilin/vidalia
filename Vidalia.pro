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

OBJECTS_DIR = obj
RCC_DIR = src/gen/qrc
UI_DIR  = src/gen/ui
MOC_DIR = src/gen/moc

TEMPLATE = app
CONFIG  += qt thread warn_on
QT      += network xml
QTPLUGIN += qjpeg

# This fixes a bug with Qt 4.1.0 not including this in the path
QMAKE_LFLAGS += -L$$[QT_INSTALL_PLUGINS]/imageformats

isEmpty(PREFIX) {
  PREFIX = /usr/local
}
isEmpty(MANDIR) {
  MANDIR = $${PREFIX}/man
}

!macx {
  # On non-Mac, make the binary all lowercase
  TARGET = vidalia
}
unix {
  # Setup the `make install` target
  target.path = $${PREFIX}/bin
  man.files = $$PWD/doc/vidalia.1
  man.path  = $${MANDIR}/man1
 INSTALLS += target man
}
win32 {
  # Link against these libraries on Win32
  LIBS += -lshell32 -lgdi32
}

include($$PWD/src/src.pri)

