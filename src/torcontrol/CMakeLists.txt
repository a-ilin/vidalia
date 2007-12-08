#####################################################################
#  $Id$
# 
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
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
#####################################################################


set(torcontrol_SRCS 
  addressmap.cpp
  circuit.cpp
  clientstatusevent.cpp
  controlcommand.cpp
  controlconnection.cpp
  controlreply.cpp
  controlsocket.cpp
  dangerousversionevent.cpp
  generalstatusevent.cpp
  logevent.cpp
  orconnevent.cpp
  protocolinfo.cpp
  replyline.cpp
  routerdescriptor.cpp
  serverstatusevent.cpp
  statusevent.cpp
  stream.cpp
  torcontrol.cpp
  torevents.cpp
  torprocess.cpp
  torsignal.cpp
)
qt4_wrap_cpp(torcontrol_SRCS
  controlconnection.h
  controlsocket.h
  torcontrol.h
  torevents.h
  torprocess.h
)

if (WIN32)
  set(torcontrol_SRCS ${torcontrol_SRCS} 
    torservice.cpp
  )
  qt4_wrap_cpp(torcontrol_SRCS
    torservice.h
  )
endif(WIN32)


add_library(torcontrol STATIC  ${torcontrol_SRCS})
target_link_libraries(torcontrol ${QT_LIBRARIES} util)

