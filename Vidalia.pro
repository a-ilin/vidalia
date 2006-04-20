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

DESTDIR = bin
OBJECTS_DIR = obj
RCC_DIR = src
UI_DIR  = src
MOC_DIR = src

TEMPLATE = app
CONFIG  += qt thread warn_on
QT      += network xml opengl

!macx {
  # On non-Mac, make the binary all lowercase
  TARGET = vidalia
}

win32 {
  # Link against these libraries on Win32
  LIBS += -lshell32 -lgdi32
}

unix {
  # Setup the `make install` target
  target.path = /usr/local/bin
  INSTALLS += target
}
  
include($$PWD/src/src.pri)

