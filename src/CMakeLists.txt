##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
##  including this file, may be copied, modified, propagated, or distributed 
##  except according to the terms described in the LICENSE file.
##


include_directories(
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/torcontrol
  ${CMAKE_CURRENT_SOURCE_DIR}/common
)
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/torcontrol
  ${CMAKE_CURRENT_BINARY_DIR}/common
)

if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
 
  ## Qt's Visual Studio builds explicitly disable treating wchar_t as a
  ## built-in type, so we need to do that as well otherwise we get linker
  ## errors due to name mangling if we try to use a wchar_t-related method
  add_definitions(/Zc:wchar_t-)

  ## Specify that we want the "W" version of Windows methods, such as
  ## CreateFileW or WriteFileW
  add_definitions(-DUNICODE=1)

  ## Disable incremental linking for all build types, not just Release
  ## and MinSizeRel
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG
      "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO"
  )
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
      "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO"
  )
endif(MSVC)

## Add some Qt definitions
if (WIN32)
  add_definitions(-DQT_THREAD_SUPPORT)
  if (NOT QT_CONFIG MATCHES "static")
    add_definitions(-DQT_DLL)
  endif(NOT QT_CONFIG MATCHES "static")
endif(WIN32)

if (USE_MINIUPNPC)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/miniupnpc)
  link_directories(${CMAKE_CURRENT_BINARY_DIR}/miniupnpc)
  add_subdirectory(miniupnpc)
endif(USE_MINIUPNPC)
if (USE_BREAKPAD)
  add_subdirectory(crashreporter)
endif(USE_BREAKPAD)

add_subdirectory(common)
add_subdirectory(tools)
add_subdirectory(torcontrol)
add_subdirectory(vidalia)
if (BUILD_TESTS)
  add_subdirectory(tests)
endif (BUILD_TESTS)
