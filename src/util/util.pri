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

HEADERS += $$PWD/net.h \
           $$PWD/http.h \
           $$PWD/string.h \
           $$PWD/torsocket.h \
           $$PWD/html.h
           
SOURCES += $$PWD/net.cpp \
           $$PWD/http.cpp \
           $$PWD/string.cpp \
           $$PWD/torsocket.cpp \
           $$PWD/html.cpp

win32 {
    HEADERS += $$PWD/registry.h \
               $$PWD/win32.h
    SOURCES += $$PWD/registry.cpp \
               $$PWD/win32.cpp
}

