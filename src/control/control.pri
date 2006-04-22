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

HEADERS += $$PWD/torcontrol.h \
           $$PWD/torprocess.h \
           $$PWD/controlconnection.h \
           $$PWD/controlsocket.h \
           $$PWD/controlcommand.h \
           $$PWD/controlreply.h \
           $$PWD/replyline.h \
           $$PWD/torevents.h \
           $$PWD/eventtype.h \
           $$PWD/bandwidthevent.h \
           $$PWD/circuitevent.h \
           $$PWD/streamevent.h \
           $$PWD/logevent.h \
           $$PWD/orconnevent.h \
           $$PWD/torsignal.h \
           $$PWD/newdescriptorevent.h \
           $$PWD/routerdescriptor.h

SOURCES += $$PWD/torcontrol.cpp \
           $$PWD/torprocess.cpp \
           $$PWD/controlconnection.cpp \
           $$PWD/controlsocket.cpp \
           $$PWD/controlcommand.cpp \
           $$PWD/controlreply.cpp \
           $$PWD/replyline.cpp \
           $$PWD/torevents.cpp \
           $$PWD/bandwidthevent.cpp \
           $$PWD/circuitevent.cpp \
           $$PWD/streamevent.cpp \
           $$PWD/logevent.cpp \
           $$PWD/orconnevent.cpp \
           $$PWD/torsignal.cpp \
           $$PWD/newdescriptorevent.cpp \
           $$PWD/routerdescriptor.cpp

