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


include_directories(
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/torcontrol
  ${CMAKE_CURRENT_SOURCE_DIR}/util
)
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/torcontrol
  ${CMAKE_CURRENT_BINARY_DIR}/util
)

if (APPLE)
  if (OSX_FAT_BINARY)
    ## Set the appropriate flags for building a Universal binary
    set(CMAKE_OSX_ARCHITECTURES ppc i386)
  endif(OSX_FAT_BINARY)
endif(APPLE)

add_subdirectory(torcontrol)
add_subdirectory(util)
add_subdirectory(vidalia)

