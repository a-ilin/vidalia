##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.vidalia-project.net/. No part of Vidalia, including this file,
##  may be copied, modified, propagated, or distributed except according to
##  the terms described in the LICENSE file.
##


set(VER_MAJOR   "0")
set(VER_MINOR   "2")
set(VER_PATCH   "0")
set(VERSION     "${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}-svn")
message(STATUS  "Configuring Vidalia ${VERSION}")
project(Vidalia)

## Specify the minimim required CMake version
cmake_minimum_required(VERSION 2.4.0)
if (COMMAND cmake_policy)
  # Force CMake 2.4 compatibility for handling linker search paths
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

## We declare this option here, because it determines the minimum
## required Qt version
option(USE_MARBLE "Enable the KDE Marble-based map widget." OFF)

## Specify the minimum version of Qt required
if (USE_MARBLE)
  set(QT_MIN_VERSION    "4.3.0")
else(USE_MARBLE)
  set(QT_MIN_VERSION    "4.2.0")
endif(USE_MARBLE)

## Specify the Qt libraries used
include(FindQt4)
find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK  true)
set(QT_USE_QTXML      true)
if (USE_MARBLE)
  set(QT_USE_QTSVG    true)
  set(QT_USE_QTWEBKIT true)
  set(QT_USE_QTSCRIPT true)
  set(QT_USE_QTDBUS   true)
endif(USE_MARBLE)
include(${QT_USE_FILE})
include(${CMAKE_SOURCE_DIR}/cmake/VidaliaMacros.cmake)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckTypeSize)
if (WIN32)
  include(${CMAKE_SOURCE_DIR}/cmake/FindWiX.cmake)
endif(WIN32)

if(MSVC_IDE)
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif(MSVC_IDE)
  
## Define Vidalia-specific CMake options
if (APPLE)
  option(OSX_FAT_BINARY "Build Vidalia as a Universal binary." OFF)
  if (OSX_FAT_BINARY)
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386" 
        CACHE STRING "OS X build architectures" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.4"
        CACHE STRING "Flags used by the linker." FORCE)
  endif(OSX_FAT_BINARY)
endif(APPLE)

## Check for QSslSocket
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${QT_INCLUDES})
check_include_file_cxx("qsslsocket.h" HAVE_QSSLSOCKET_H)
if (HAVE_QSSLSOCKET_H)
  check_symbol_exists(QT_NO_OPENSSL "QtGlobal" QT_NO_SSL_SUPPORT)
  if (NOT QT_NO_SSL_SUPPORT)
    option(USE_QSSLSOCKET "Use Qt's QSslSocket for GeoIP lookups." ON)
  endif(NOT QT_NO_SSL_SUPPORT)
endif(HAVE_QSSLSOCKET_H)
if (USE_QSSLSOCKET)
  if (MSVC OR UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/FindOpenSSL.cmake)
  endif(MSVC OR UNIX)
endif(USE_QSSLSOCKET)

## UPnP support is currently optional (enabled by default)
option(USE_MINIUPNPC "Enable UPnP support using the MiniUPnPc library." ON)

## Automatic osftware update is optional (disabled by default)
if (WIN32)
  option(USE_AUTOUPDATE "Enable automatic software update support." OFF)
endif(WIN32)

## Find the KDE Marble library
if (USE_MARBLE)
  include(${CMAKE_SOURCE_DIR}/cmake/FindMarble.cmake)
endif(USE_MARBLE)

## Check for system header files
check_include_file("limits.h" HAVE_LIMITS_H)
check_include_file("sys/limits.h" HAVE_SYS_LIMITS_H)
check_include_file("math.h"  HAVE_MATH_H)
check_include_file_cxx("cmath" HAVE_CMATH)

## Check for the sizes of various data types
check_type_size(int SIZEOF_INT)

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

