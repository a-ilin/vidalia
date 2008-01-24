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
set(VER_MAJOR   "0")
set(VER_MINOR   "1")
set(VER_PATCH   "0")
set(VERSION     "${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}-svn")

## Specify the minimim required CMake version
cmake_minimum_required(VERSION 2.4.0)

## Require Qt >= 4.2.0
set(QT_MIN_VERSION    "4.2.0")

## Specify the Qt libraries used
include(FindQt4)
find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK  true)
set(QT_USE_QTXML      true)
include(${QT_USE_FILE})
include(VidaliaMacros.cmake)
include(CheckIncludeFileCXX)
include(CPack)

## Define Vidalia-specific CMake options
if (APPLE)
  option(OSX_FAT_BINARY "Build Vidalia as a Universal binary." OFF)
endif(APPLE)

## Check for QSystemTrayIcon
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${QT_INCLUDES})
if (NOT APPLE)
  check_include_file_cxx("qsystemtrayicon.h" HAVE_QSYSTEMTRAYICON_H)
  if (HAVE_QSYSTEMTRAYICON_H)
    option(USE_QSYSTEMTRAYICON "Use Qt's tray icon implementation." ON)
  endif(HAVE_QSYSTEMTRAYICON_H)
endif(NOT APPLE)

## Check for QSslSocket
check_include_file_cxx("qsslsocket.h" HAVE_QSSLSOCKET_H)
if(HAVE_QSSLSOCKET_H)
  check_symbol_exists(QT_NO_OPENSSL "QtGlobal" QT_NO_SSL_SUPPORT)
  if(NOT QT_NO_SSL_SUPPORT)
    option(USE_QSSLSOCKET "Use Qt's QSslSocket for GeoIP lookups." ON)
  endif(NOT QT_NO_SSL_SUPPORT)
endif(HAVE_QSSLSOCKET_H)

## Write out a configuration file
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

## Add the actual source directories
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(pkg)

## The 'confclean' target removes cached CMake configuration information
if (WIN32)
  set(RM_CMD del)
else (WIN32)
  set(RM_CMD rm)
endif(WIN32)
add_custom_target(confclean ${RM_CMD} CMakeCache.txt)

