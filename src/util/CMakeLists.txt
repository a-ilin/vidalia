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


set(util_SRCS
  crypto.cpp
  file.cpp
  html.cpp
  log.cpp
  net.cpp
  process.cpp
  stringutil.cpp
  torsocket.cpp
  zlibbytearray.cpp
)
qt4_wrap_cpp(util_SRCS torsocket.h)


if(USE_QSSLSOCKET)
  set(util_SRCS ${util_SRCS} torsslsocket.cpp)
  qt4_wrap_cpp(util_SRCS torsslsocket.h)
endif(USE_QSSLSOCKET)

if(WIN32)
  set(util_SRCS ${util_SRCS}
    win32.cpp
  )
endif(WIN32)


add_library(util STATIC ${util_SRCS})
target_link_libraries(util ${QT_LIBRARIES})

if(USE_QSSLSOCKET AND OPENSSL_LIBRARIES)
  target_link_libraries(util ${OPENSSL_LIBRARIES})
endif(USE_QSSLSOCKET AND OPENSSL_LIBRARIES)

