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


project(Vidalia)
set(VERSION "0.1.0-svn")

## Specify the minimim required CMake version
cmake_minimum_required(VERSION 2.4.0)

## Require Qt >= 4.1.0
set(QT_MIN_VERSION    "4.1.0")

## Specify the Qt libraries used
include(FindQt4)
find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK  true)
set(QT_USE_QTXML      true)
include(${QT_USE_FILE})
include(VidaliaMacros.cmake)

## Define Vidalia-specific CMake options
option(OSX_FAT_BINARY "Build Vidalia as a Universal binary." OFF)

## Add the actual source directories
add_subdirectory(src)
add_subdirectory(doc)

## The 'confclean' target removes cached CMake configuration information
if (WIN32)
  set(RM_CMD del)
else (WIN32)
  set(RM_CMD rm)
endif(WIN32)
add_custom_target(confclean ${RM_CMD} CMakeCache.txt)

