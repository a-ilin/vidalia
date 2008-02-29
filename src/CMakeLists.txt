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

if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
endif(MSVC)

## Add some Qt definitions
if (WIN32)
  add_definitions(-DQT_THREAD_SUPPORT)
  if (NOT QT_CONFIG MATCHES "static")
    add_definitions(-DQT_DLL)
  endif(NOT QT_CONFIG MATCHES "static")
endif(WIN32)

add_subdirectory(torcontrol)
add_subdirectory(util)
add_subdirectory(vidalia)

