#################################################################
#  $Id: network.pri 422 2006-03-13 05:13:00 hipplej$
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

FORMS += $$PWD/netviewer.ui

HEADERS += $$PWD/netviewer.h \
           $$PWD/zimageview.h \
           $$PWD/tormapwidget.h \
           $$PWD/routerlistwidget.h \
           $$PWD/routerlistitem.h \
           $$PWD/routerdescriptorview.h \
           $$PWD/circuitlistwidget.h \
           $$PWD/circuititem.h \
           $$PWD/streamitem.h

SOURCES += $$PWD/netviewer.cpp \
           $$PWD/zimageview.cpp \
           $$PWD/tormapwidget.cpp \
           $$PWD/routerlistwidget.cpp \
           $$PWD/routerlistitem.cpp \
           $$PWD/routerdescriptorview.cpp \
           $$PWD/circuitlistwidget.cpp \
           $$PWD/circuititem.cpp \
           $$PWD/streamitem.cpp

