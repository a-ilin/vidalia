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
set(VER_PATCH   "11")
set(VERSION     "${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}")
message(STATUS  "Configuring Vidalia ${VERSION}")
project(Vidalia)

## Specify the minimim required CMake version
cmake_minimum_required(VERSION 2.4.7)
if (COMMAND cmake_policy)
  # Force CMake 2.4 compatibility for handling linker search paths
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

## We declare this option here, because it determines the minimum
## required Qt version
option(USE_MARBLE "Enable the KDE Marble-based map widget." OFF)

## Specify the minimum version of Qt required
set(QT_MIN_VERSION    "4.3.0")

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
include(CheckFunctionExists)
if (WIN32)
  include(${CMAKE_SOURCE_DIR}/cmake/FindWiX.cmake)
endif(WIN32)
if (MSVC)
  include(${CMAKE_SOURCE_DIR}/cmake/FindOpenSSL.cmake)
endif(MSVC)

if(MSVC_IDE)
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif(MSVC_IDE)
  
## Define Vidalia-specific CMake options
if (APPLE)
  option(OSX_TIGER_COMPAT "Build an OS X 10.4 Compatible Vidalia." OFF)
  if (OSX_TIGER_COMPAT)
    set(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.4"
      CACHE STRING "Flags used by the linker." FORCE)
    set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk"
      CACHE STRING "isysroot used for universal binary support" FORCE)
  endif(OSX_TIGER_COMPAT)

  option(OSX_FAT_BINARY "Build Vidalia as a Universal binary." OFF)
  if (OSX_FAT_BINARY)
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386" 
        CACHE STRING "OS X build architectures" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.4"
        CACHE STRING "Flags used by the linker." FORCE)
    set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk"
        CACHE STRING "isysroot used for universal binary support" FORCE)
  endif(OSX_FAT_BINARY)

  option(OSX_FORCE_32BIT "Force a 32-bit build for compatibility." OFF)
  if (OSX_FORCE_32BIT)
    add_definitions(-m32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32" 
        CACHE STRING "Flags used by the linker." FORCE)
  endif(OSX_FORCE_32BIT)
endif(APPLE)

## Define if compiled with Windows 2k support
option(WIN2K "Enable Windows 2k support." OFF)

## UPnP support is currently optional (enabled by default)
option(USE_MINIUPNPC "Enable UPnP support using the MiniUPnPc library." ON)

## Crash reporting via Google Breakpad is optional (disabled by default)
option(USE_BREAKPAD "Enable Google Breakpad crash reporting." OFF)
if (USE_BREAKPAD)
  include(${CMAKE_SOURCE_DIR}/cmake/FindBreakpad.cmake)
endif(USE_BREAKPAD)

## Automatic software update is optional (disabled by default)
if (WIN32)
  option(USE_AUTOUPDATE "Enable automatic software update support." OFF)
endif(WIN32)

## Find the KDE Marble library
if (USE_MARBLE)
  include(${CMAKE_SOURCE_DIR}/cmake/FindMarble.cmake)
endif(USE_MARBLE)

## Find the MaxMind GeoIP library
option(USE_GEOIP "Enable GeoIP lookups via a local MaxMind database" OFF)
if (USE_GEOIP)
  include(${CMAKE_SOURCE_DIR}/cmake/FindGeoIP.cmake)
endif(USE_GEOIP)

## Check for system header files
check_include_file("limits.h" HAVE_LIMITS_H)
check_include_file("sys/limits.h" HAVE_SYS_LIMITS_H)
check_include_file("signal.h" HAVE_SIGNAL_H)

## Check for the sizes of various data types
check_type_size(int SIZEOF_INT)

## Check for the existence of some platform-specific functions
if (HAVE_SIGNAL_H)
  set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} "signal.h")
  check_function_exists(sigaction HAVE_SIGACTION)
  check_function_exists(signal HAVE_SIGNAL)
endif(HAVE_SIGNAL_H)

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

